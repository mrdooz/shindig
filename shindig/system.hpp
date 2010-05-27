#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include <fmod.hpp>

typedef fastdelegate::FastDelegate1<const std::string&, bool> fnFileChanged;
typedef fastdelegate::FastDelegate2<float, float, void> FrequencyCallback;

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

struct Demo;


class System
{
public:

  enum DirTag {
    kDirDropBox,
    kDirRelative,
    kDirAbsolute,
  };

  struct Freq
  {
    Freq(float min_freq, float max_freq, float amp, const FrequencyCallback& cb) : min_freq(min_freq), max_freq(max_freq), amp(amp), cb(cb), ticked(false) {}
    float min_freq;
    float max_freq;
    float amp;
    FrequencyCallback cb;
    bool ticked;
  };

	static System& instance();
	bool init();
	bool close();
	bool tick();



	//boost::signals2::connection add_file_changed(const fnFileChanged& slot);
	//boost::signals2::connection add_file_changed(const std::string& filename, const fnFileChanged& slot, const bool initial_load = false);

  bool add_file_changed(const fnFileChanged& slot);
  bool add_file_changed(const std::string& filename, const fnFileChanged& slot, const bool initial_load = false);

  const std::string& dropbox() const { return _dropbox; }
  const std::string& working_dir() const { return _working_dir; }

  std::string convert_path(const std::string& str, DirTag tag);

  void add_error_message(const char* fmt, ...);
  void add_callback(const Freq& f);

	bool start_mp3();
	bool end_mp3();
	bool paused();
	void set_paused(const bool state);

  std::string error_message() const;

  void set_demo(const Demo* demo);

  void  load_callback_times(const char *filename);


private:
	DISALLOW_COPY_AND_ASSIGN(System);
	System();
	~System();

	typedef boost::signals2::signal< void(std::string) > sigFileChanged;
	typedef std::map< std::string, boost::shared_ptr<sigFileChanged> > SpecificSignals;
	typedef std::set< std::string > DeferredFiles;

  struct TimeStamp
  {
    float time;
    float value;
  };

	static DWORD WINAPI WatcherThread(void* param);
	void file_changed_internal(const std::string& filename);
  void enum_known_folders();

  void process_frequency_callbacks();
  void process_timed_callbacks();


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
  std::vector<uint32_t>  _callback_times;
	FMOD::System* _fmod_system;
	FMOD::Channel* _channel;
	FMOD::Sound* _sound;

  std::string _error_msg;

  std::vector<Freq> _frequency_callbacks;
  float *_spectrum_left;
  float *_spectrum_right;
  float *_spectrum_combined;


  Demo *_demo;
};

#endif
