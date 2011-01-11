#include "stdafx.h"
#include "system.hpp"

/*
  TODO: load a shindig.conf, and a local shindig.user.conf
*/

System* System::_instance = nullptr;

System::System()
	: _fmod_system(NULL)
	, _channel(NULL)
	, _sound(NULL)
	, _time_idx(0)
  , _spectrum_left(new float[1024])
  , _spectrum_right(new float[1024])
  , _spectrum_combined(new float[1024])
{
  _debug_host = "192.168.0.17";
}

System::~System()
{
  SAFE_ADELETE(_spectrum_left);
  SAFE_ADELETE(_spectrum_right);
  SAFE_ADELETE(_spectrum_combined);
  delete this;
  _instance = nullptr;
}

System& System::instance()
{
	if (_instance == NULL)
		_instance = new System();
	return *_instance;
}

bool System::init()
{
  enum_known_folders();

  if (!FileWatcher::instance().init())
    return false;


	if (FMOD::System_Create(&_fmod_system) != FMOD_OK)
		return false;

	if (_fmod_system->init(32, FMOD_INIT_NORMAL, 0) != FMOD_OK)
		return false;

	if (_fmod_system->createSound(convert_path("data/mp3/12 Session.mp3", System::kDirDropBox), FMOD_SOFTWARE, 0, &_sound) != FMOD_OK)
		return false;

	return true;
}

bool System::close()
{
  if (!FileWatcher::instance().close())
    return false;

	return true;
}

bool System::tick()
{
  FileWatcher::instance().tick();    

	if (_channel) {
    process_frequency_callbacks();
    //process_timed_callbacks();
	}

	return true;
}

void System::process_frequency_callbacks()
{
  _fmod_system->getSpectrum(_spectrum_left, 1024, 0, FMOD_DSP_FFT_WINDOW_RECT);
  _fmod_system->getSpectrum(_spectrum_right, 1024, 0, FMOD_DSP_FFT_WINDOW_RECT);

  for (int i = 0; i < 1024; ++i)
    _spectrum_combined[i] = 0.5f * (_spectrum_left[i] + _spectrum_right[i]);

  for (int i = 0; i < (int32_t)_frequency_callbacks.size(); ++i)
    _frequency_callbacks[i].ticked = false;

  const float inc = 44100.0f / 2 / (1024 - 1);
  float lower_hz = 0;
  float upper_hz = inc;
  for (int i = 0; i < 1024; ++i) {

    for (int j = 0; j < (int)_frequency_callbacks.size(); ++j) {
      auto &cur = _frequency_callbacks[j];
      if (cur.ticked)
        continue;
      const bool freq_ok = (cur.min_freq >= lower_hz && cur.min_freq < upper_hz) || 
        (cur.max_freq >= lower_hz && cur.max_freq < upper_hz) ||
        (cur.min_freq <= lower_hz && cur.max_freq > upper_hz);
      const bool amp_ok = _spectrum_combined[i] >= cur.amp;
      if (freq_ok && amp_ok) {
        cur.cb(0.5f * (lower_hz + upper_hz), _spectrum_combined[i]);
        cur.ticked = true;
      }
    }
    lower_hz += inc;
    upper_hz += inc;
  }
}

void System::process_timed_callbacks()
{
  if (_callback_times.empty() || _frequency_callbacks.empty())
    return;

  uint32_t cur_time = 0;
  _channel->getPosition(&cur_time, FMOD_TIMEUNIT_MS);

  if (cur_time >= _callback_times[_time_idx])
    _frequency_callbacks[0].cb(0,0);

  while (cur_time >= _callback_times[_time_idx] && _time_idx < _callback_times.size())
    _time_idx++;

  if (_time_idx == _callback_times.size())
    return;

}

// Loads callbacks from a file containing a list of floats (exported from Sonic Visualiser)
void System::load_callback_times(const char *filename)
{
  FILE * f = fopen(filename, "rt");
  if (!f)
    return;

  float t;
  while (fscanf(f, "%f", &t) == 1)
    _callback_times.push_back((int32_t)(1000 * t));

  fclose(f);
}

bool System::add_file_changed(const string2& filename, const fnFileChanged& fn, const bool initial_load)
{
  return FileWatcher::instance().add_file_changed(filename, fn, initial_load);
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
  RETURN_ON_FAIL_VOID(CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm)), LOG_WARNING_LN);
  KNOWNFOLDERID id;
  RETURN_ON_FAIL_VOID(pkfm->FolderIdFromCsidl(CSIDL_MYDOCUMENTS, &id), LOG_WARNING_LN);
  CComPtr<IKnownFolder> k;
  RETURN_ON_FAIL_VOID(pkfm->GetFolder(id, &k), LOG_WARNING_LN);
  KNOWNFOLDER_DEFINITION kfd;
  RETURN_ON_FAIL_VOID(k->GetFolderDefinition(&kfd), LOG_WARNING_LN);
  PWSTR pszPath = NULL;
  if (SUCCEEDED(k->GetPath(0, &pszPath))) {
    char* str = NULL;
    UnicodeToAnsi(pszPath, &str);
    _my_documents = str;
    _dropbox = _my_documents + "\\My DropBox\\";
    if( !(GetFileAttributesA(_dropbox) & FILE_ATTRIBUTE_DIRECTORY)) {
      _dropbox = "";
    }

    free(str);
    CoTaskMemFree(pszPath);
  }
  FreeKnownFolderDefinitionFields(&kfd);
}

string2 System::convert_path(const string2& str, DirTag tag)
{
  string2 res;
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
		case kDirCustom:
			// search the custom dirs until we find a matching file..
			for (int i = 0; i < (int)_custom_dirs.size(); ++i) {
				string2 t = _custom_dirs[i] + str;
				if (file_exists(t))
					return t;
			}
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

void System::add_callback(const Freq& f)
{
  _frequency_callbacks.push_back(f);
}

void System::add_custom_dir(const string2& dir)
{
	_custom_dirs.push_back(dir);
}
