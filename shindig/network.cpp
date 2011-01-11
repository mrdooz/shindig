#include "stdafx.h"
#include "network.hpp"
#include "system.hpp"

#pragma comment(lib, "ws2_32.lib")

Network *Network::_instance = nullptr;

Network& Network::instance()
{
  if (!_instance)
    _instance = new Network();
  return *_instance;
}

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
static void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
    return &(((sockaddr_in*)sa)->sin_addr);

  return &(((sockaddr_in6*)sa)->sin6_addr);
}

Network::Network()
  : _fd(~0)
  , _network_thread(INVALID_HANDLE_VALUE)
	, _close_event(INVALID_HANDLE_VALUE)
	, _recv_event(INVALID_HANDLE_VALUE)
{
}


DWORD Network::network_thread(void *param)
{
  Network *self = (Network *)param;
  return self->network_thread_local();
}

DWORD Network::network_thread_local()
{
  char buf[256];
  WSABUF wsabuf;

  // async receive

  WSAOVERLAPPED overlapped;
  HANDLE events[] = { _close_event, _recv_event };
  while (true) {
    DWORD bytes_read = 0, flags = 0;
    ZeroMemory(&overlapped, sizeof(overlapped));
    overlapped.hEvent = _recv_event;
    wsabuf.len = 256;
    wsabuf.buf = buf;
    int res = WSARecv(_fd, &wsabuf, 1, &bytes_read, &flags, &overlapped, NULL);
    if ( !(res == 0 || res == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))
      return 0;

    // wait for the event
    int idx = WSAWaitForMultipleEvents(2, events, FALSE, WSA_INFINITE, FALSE);
    // break if we get a close event
    if (idx - WSA_WAIT_EVENT_0 == 0)
      break;

    WSAResetEvent(_recv_event);

    WSAGetOverlappedResult(_fd, &overlapped, &bytes_read, FALSE, &flags);
    int a = 10;

    // process the data..
  }

  return 0;
}

bool Network::init()
{
  WSADATA wsaData;

  // MAKEWORD(2,0) for Winsock 1.1, MAKEWORD(2,0) for Winsock 2.0:
  if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
    fprintf(stderr, "WSAStartup failed.\n");
    return false;
  }

  addrinfo hints;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int res;
  addrinfo *servinfo;
  if ((res = getaddrinfo(System::instance().debug_host(), PORT, &hints, &servinfo)) != 0) {
    LOG_WARNING_LN("getaddrinfo: %s\n", gai_strerror(res));
    return true;
  }

  // loop through all the results and connect to the first we can
  addrinfo *p;
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((_fd = WSASocket(p->ai_family, p->ai_socktype, p->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
      continue;

    if (connect(_fd, p->ai_addr, p->ai_addrlen) != -1)
      break;

    closesocket(_fd);
  }

  if (p == NULL) {
    LOG_WARNING_LN("Unable to connect to server");
    return true;
  }

  char s[INET6_ADDRSTRLEN];
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
  LOG_INFO_LN("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  // create the recv and close events
  _recv_event = CreateEvent(NULL, FALSE, FALSE, NULL);
  _close_event = CreateEvent(NULL, FALSE, FALSE, NULL);

  // start the network thread
  _network_thread = CreateThread(NULL, 0, &Network::network_thread, (void *)this, 0, NULL);

  return true;
}

bool Network::close()
{
  if (_fd != ~0) {
    closesocket(_fd);
    _fd = ~0;
  }

  if (_network_thread != INVALID_HANDLE_VALUE) {
    SetEvent(_close_event);
    WaitForSingleObject(_network_thread, INFINITE);
  }

	if (_close_event != INVALID_HANDLE_VALUE)
		CloseHandle(_close_event);

	if (_recv_event != INVALID_HANDLE_VALUE)
		CloseHandle(_recv_event);

  WSACleanup();

  return true;
}
