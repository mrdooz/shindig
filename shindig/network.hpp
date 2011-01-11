#ifndef _NETWORK_HPP_
#define _NETWORK_HPP_

class Network
{
public:
  static Network& instance();

  bool init();
  bool close();

  bool connected() const { return _fd != ~0; }

private:
  Network();
  static DWORD __stdcall network_thread(void *param);
  DWORD network_thread_local();

  HANDLE _close_event;
  HANDLE _recv_event;
  HANDLE _network_thread;
  static Network *_instance;
  SOCKET _fd;

};


#endif
