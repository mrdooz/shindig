#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include <fmod.hpp>

typedef std::function<bool (const string2&)> fnFileChanged;
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
		kDirCustom,
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

  bool add_file_changed(const string2& filename, const fnFileChanged& fn, const bool initial_load);

  const string2& dropbox() const { return _dropbox; }
  const string2& working_dir() const { return _working_dir; }

	void add_custom_dir(const string2& dir);

  string2 convert_path(const string2& str, DirTag tag);

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

	typedef std::map< string2, std::vector<fnFileChanged> > FileChangedCallbacks;
	typedef std::set< std::string > DeferredFiles;

  struct TimeStamp
  {
    float time;
    float value;
  };

  void enum_known_folders();

  void process_frequency_callbacks();
  void process_timed_callbacks();

	DWORD _main_thread_id;
	static System* _instance;

  string2 _my_documents;
  string2 _dropbox;
  string2 _working_dir;

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

	std::vector<string2> _custom_dirs;

  Demo *_demo;
};

#endif
