#include "stdafx.h"
#include "system.hpp"
#include "fast_delegate.hpp"
#include <celsus/UnicodeUtils.hpp>
#include <shobjidl.h>
#include <Shlguid.h>
#include <Shlobj.h>
#include <direct.h>

using namespace boost::signals2;

System* System::_instance = NULL;

enum {
	COMPLETION_KEY_NONE         =   0,
	COMPLETION_KEY_SHUTDOWN     =   1,
	COMPLETION_KEY_IO           =   2
};


/*
  Directory watcher thread. Uses completion ports to block until it detects a change in the directory tree
  or until a shutdown event is posted
*/

DWORD WINAPI System::WatcherThread(void* param)
{
	System& sys = System::instance();
  const int32_t NUM_ENTRIES = 128;
  FILE_NOTIFY_INFORMATION info[NUM_ENTRIES];

  while (true) {

		sys._dir_handle = CreateFile("./", FILE_LIST_DIRECTORY, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

    if (sys._dir_handle == INVALID_HANDLE_VALUE) {
      return 1;
    }

    sys._watcher_completion_port = CreateIoCompletionPort(sys._dir_handle, NULL, COMPLETION_KEY_IO, 0);

    if (sys._watcher_completion_port == INVALID_HANDLE_VALUE) {
      return 1;
    }

    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(overlapped));
    const BOOL res = ReadDirectoryChangesW(sys._dir_handle, info, sizeof(info), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &overlapped, NULL);
    if (!res) {
      return 1;
    }

    DWORD bytes;
    ULONG key = COMPLETION_KEY_NONE;
    OVERLAPPED *overlapped_ptr = NULL;
    bool done = false;
    GetQueuedCompletionStatus(sys._watcher_completion_port, &bytes, &key, &overlapped_ptr, INFINITE);
    switch (key) {
    case COMPLETION_KEY_NONE: 
      done = true;
      break;
    case COMPLETION_KEY_SHUTDOWN: 
      return 1;

    case COMPLETION_KEY_IO: 
      break;
    }
    CloseHandle(sys._dir_handle);
    CloseHandle(sys._watcher_completion_port);

    if (done) {
      break;
    } else {
      info[0].FileName[info[0].FileNameLength/2+0] = 0;
      info[0].FileName[info[0].FileNameLength/2+1] = 0;
			
			char tmp[MAX_PATH];
			UnicodeToAnsiToBuffer(info[0].FileName, tmp, MAX_PATH);
			const std::string filename(Path::make_canonical(Path::get_full_path_name(tmp)));
			System::instance().file_changed_internal(filename);
    }
  }
  return 0;
}

System::System()
	: _fmod_system(NULL)
	, _channel(NULL)
	, _sound(NULL)
	, _time_idx(0)
  , _spectrum_left(new float[1024])
  , _spectrum_right(new float[1024])
{
}

System::~System()
{
  SAFE_ADELETE(_spectrum_left);
  SAFE_ADELETE(_spectrum_right);
}

System& System::instance()
{
	if (_instance == NULL) {
		_instance = new System();
	}
	return *_instance;
}

bool System::init()
{
  enum_known_folders();

	DWORD thread_id;
	_watcher_thread = CreateThread(0, 0, WatcherThread, NULL, 0, &thread_id);
	InitializeCriticalSection(&_cs_deferred_files);

	if (FMOD::System_Create(&_fmod_system) != FMOD_OK)
		return false;

	if (_fmod_system->init(32, FMOD_INIT_NORMAL, 0) != FMOD_OK)
		return false;

	if (_fmod_system->createSound(convert_path("data/mp3/12 Session.mp3", System::kDirDropBox).c_str(), FMOD_HARDWARE, 0, &_sound) != FMOD_OK)
		return false;

  load_timestamps(convert_path("data/mp3/session.dat", System::kDirDropBox).c_str());

	return true;
}

bool System::close()
{
	_specific_signals.clear();
	DeleteCriticalSection(&_cs_deferred_files);

	PostQueuedCompletionStatus(_watcher_completion_port, 0, COMPLETION_KEY_SHUTDOWN, 0);
	WaitForSingleObject(_watcher_thread, INFINITE);

	return true;
}

bool System::tick()
{
	// process the deferred files
	SCOPED_CS(&_cs_deferred_files);

	if (!_deferred_files.empty()) {
		for (DeferredFiles::iterator i = _deferred_files.begin(), e = _deferred_files.end(); i != e; ++i) {
			const std::string& filename = *i;
			_global_signals(filename);

			for (SpecificSignals::iterator i = _specific_signals.begin(), e = _specific_signals.end(); i != e; ++i) {
				if (i->first == filename) {
					(*i->second)(filename);
				}
			}
		}

		_deferred_files.clear();
	}


	if (_channel) {
		if (_cb) {
			uint32_t tmp;
			_channel->getPosition(&tmp, FMOD_TIMEUNIT_MS);
      const float pos = tmp / 1000.0f;

      if (pos >= _timestamps[_time_idx].time)
        _cb(0);

      while (pos > _timestamps[_time_idx].time && _time_idx <= _timestamps.size())
        _time_idx++;

		}
	}

	return true;
}


void System::load_timestamps(const char *filename)
{
  _timestamps.clear();

  HANDLE h = CreateFile(filename, GENERIC_READ, NULL, NULL, OPEN_EXISTING, 0, NULL);
  if (h == INVALID_HANDLE_VALUE)
    return;

  int32_t count = 0;
  DWORD bytes_read = 0;
  ReadFile(h, (void*)&count, sizeof(count), &bytes_read, NULL);

  _timestamps.resize(count);
  ReadFile(h, (void*)&_timestamps[0], sizeof(TimeStamp) * count, &bytes_read, NULL);

  CloseHandle(h);
}

void System::file_changed_internal(const std::string& filename)
{
	// queue the file change
	SCOPED_CS(&_cs_deferred_files);
	_deferred_files.insert(filename);

}

boost::signals2::connection System::add_file_changed(const fnFileChanged& slot)
{
	return _global_signals.connect(slot);
}

boost::signals2::connection System::add_file_changed(const std::string& filename, const fnFileChanged& slot)
{
	auto f = Path::make_canonical(Path::get_full_path_name(filename));
	SpecificSignals::iterator it = _specific_signals.find(f);
	if (it == _specific_signals.end()) {
		_specific_signals.insert(std::make_pair(f, new sigFileChanged()));
	}
	return _specific_signals[f]->connect(slot);
}

void System::enum_known_folders()
{

  char buf[MAX_PATH+1];
  _getcwd(buf, MAX_PATH);
  _working_dir = buf;
  _working_dir += "\\";

  // find the "my documents" and drop box folders
  // See https://cfx.svn.codeplex.com/svn/Visual%20Studio%202008/CppShellKnownFolders/ReadMe.txt
  // for a good description of how this stuff works
  CComPtr<IKnownFolderManager> pkfm;
  RETURN_ON_FAIL_VOID(CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm)), ErrorPredicate<HRESULT>, LOG_WARNING_LN);
  KNOWNFOLDERID id;
  RETURN_ON_FAIL_VOID(pkfm->FolderIdFromCsidl(CSIDL_MYDOCUMENTS, &id), ErrorPredicate<HRESULT>, LOG_WARNING_LN);
  CComPtr<IKnownFolder> k;
  RETURN_ON_FAIL_VOID(pkfm->GetFolder(id, &k), ErrorPredicate<HRESULT>, LOG_WARNING_LN);
  KNOWNFOLDER_DEFINITION kfd;
  RETURN_ON_FAIL_VOID(k->GetFolderDefinition(&kfd), ErrorPredicate<HRESULT>, LOG_WARNING_LN);
  PWSTR pszPath = NULL;
  if (SUCCEEDED(k->GetPath(0, &pszPath))) {
    char* str = NULL;
    UnicodeToAnsi(pszPath, &str);
    _my_documents = str;
    _dropbox = _my_documents + "\\My DropBox\\";
    if( !(GetFileAttributesA(_dropbox.c_str()) & FILE_ATTRIBUTE_DIRECTORY)) {
      _dropbox = "";
    }

    free(str);
    CoTaskMemFree(pszPath);
  }
  FreeKnownFolderDefinitionFields(&kfd);
}

std::string System::convert_path(const std::string& str, DirTag tag)
{
  std::string res;
  switch(tag) {
    case kDirRelative:
      res = working_dir() + str;
      break;
    case kDirDropBox:
      res = dropbox() + str;
      break;
    case kDirAbsolute:
      res = str;
      break;
  }
  return res;
}

void System::add_error_message(const char* fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);

  const int len = _vscprintf(fmt, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, fmt, arg);
  va_end(arg);
  _error_msg = buf;

  LOG_ERROR_LN(buf);
}

bool System::start_mp3()
{
	if (!_sound)
		return false;

	_fmod_system->playSound(FMOD_CHANNEL_FREE, _sound, false, &_channel);

	return true;
}

bool System::end_mp3()
{
	if (!_sound || !_channel)
		return false;

	return true;
}

bool System::paused()
{
	if (!_sound || !_channel)
		return false;

	bool p = false;
	if (_channel->getPaused(&p) != FMOD_OK)
		return false;

	return p;
}

void System::set_paused(const bool state)
{
	if (!_sound || !_channel)
		return;

	_channel->setPaused(state);
}

void System::add_timed_callback(const int idx, TimedCallback& fn)
{
	_cb = fn;
}
