#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include <fmod.hpp>
//#include "../spectrum/spectrum/bin/Debug/time.hpp"

typedef fastdelegate::FastDelegate1<const std::string&, void> fnFileChanged;
typedef fastdelegate::FastDelegate1<int, void> TimedCallback;

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

class System
{
public:

  enum DirTag {
    kDirDropBox,
    kDirRelative,
    kDirAbsolute,
  };

	static System& instance();
	bool init();
	bool close();
	bool tick();

	boost::signals2::connection add_file_changed(const fnFileChanged& slot);
	boost::signals2::connection add_file_changed(const std::string& filename, const fnFileChanged& slot);

  const std::string& dropbox() const { return _dropbox; }
  const std::string& working_dir() const { return _working_dir; }

  std::string convert_path(const std::string& str, DirTag tag);

  void add_error_message(const char* fmt, ...);
	void add_timed_callback(const int idx, TimedCallback& fn);

	bool start_mp3();
	bool end_mp3();
	bool paused();
	void set_paused(const bool state);

private:
	DISALLOW_COPY_AND_ASSIGN(System);
	System();
	~System();

	typedef boost::signals2::signal< void(std::string) > sigFileChanged;
	typedef std::map< std::string, boost::shared_ptr<sigFileChanged> > SpecificSignals;
	typedef std::set< std::string > DeferredFiles;

	static DWORD WINAPI WatcherThread(void* param);
	void file_changed_internal(const std::string& filename);
  void enum_known_folders();

	CRITICAL_SECTION _cs_deferred_files;
	sigFileChanged _global_signals;
	SpecificSignals _specific_signals;
	DeferredFiles _deferred_files;

	DWORD _main_thread_id;
	HANDLE _watcher_thread;
	HANDLE _dir_handle;
	HANDLE _watcher_completion_port;
	static System* _instance;

  std::string _my_documents;
  std::string _dropbox;
  std::string _working_dir;

	uint32_t _time_idx;
	TimedCallback _cb;
	FMOD::System* _fmod_system;
	FMOD::Channel* _channel;
	FMOD::Sound* _sound;
};

#endif
