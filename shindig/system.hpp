#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

typedef fastdelegate::FastDelegate< void (const std::string&) > fnFileChanged;

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
};

#endif
