/**
 * @file
 * @brief Windows implementation of a socket
 *
 * Uses WinSock
 */
#ifndef CPEN333_PROCESS_WINDOWS_SOCKET_H
#define CPEN333_PROCESS_WINDOWS_SOCKET_H

#include <string>
#include <cstdint>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mutex>

#include "../../../util.h"

#ifdef _MSC_VER
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

#ifndef CPEN333_SOCKET_DEFAULT_PORT
#define CPEN333_SOCKET_DEFAULT_PORT 5120
#endif

namespace cpen333 {
namespace process {

namespace windows {

/**
 * @brief Singleton used for initializing and destroying WSA
 */
class WSASingleton {
 private:
  std::mutex mutex_;
  int count_;

  WSASingleton() : mutex_(), count_(0){
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result != 0) {
      cpen333::error(std::string("WSAStartup(...) failed with error: ") + std::to_string(result));
    }
    // std::cout << "WSA startup" << std::endl;
  }

  ~WSASingleton() {
    WSACleanup();
    // std::cout << "WSA cleanup" << std::endl;
  }

 public:
  void acquire() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++count_;
  }

  void release() {
    std::lock_guard<std::mutex> lock(mutex_);
    --count_;
  }

  int usage_count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
  }

  static WSASingleton& instance() {
    static WSASingleton guard;
    return guard;
  }
};

// forward declaration so client can friend it
class socket_server;

class socket_client {
 private:
  std::string server_;
  int port_;
  SOCKET socket_;

  bool open_;
  bool connected_;
  WSASingleton& wsa_;

  friend class socket_server;

  void __initialize(const std::string& server, int port,
                    SOCKET socket, bool open, bool connected) {
    server_ = server;
    port_ = port;
    socket_ = socket;
    open_ = open;
    connected_ = connected;
  }

 public:
  socket_client() : server_("localhost"), port_(CPEN333_SOCKET_DEFAULT_PORT),
                    socket_(INVALID_SOCKET), open_(false), connected_(false),
                    wsa_(WSASingleton::instance()){
    wsa_.acquire();
  }

  socket_client(const std::string& server, int port) :
      server_(server), port_(port),
      socket_(INVALID_SOCKET), open_(false), connected_(false),
      wsa_(WSASingleton::instance()) {
    wsa_.acquire();
  }

  ~socket_client() {
    close();
    wsa_.release();
  }

  bool open() {

    // don't open if already opened
    if (open_) {
      return false;
    }

    // don't open if illegal port
    if (port_ <= 0) {
      return false;
    }

    /* Obtain address(es) matching host/port */
    struct addrinfo hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    struct addrinfo *addrresult;
    std::string strport = std::to_string(port_);
    int status = getaddrinfo(server_.c_str(), strport.c_str(),
                             &hints, &addrresult);
    if (status != 0) {
      cpen333::error(std::string("getaddrinfo(...) failed with error: ")
                         + std::to_string(status));
      return false;
    }

    // Attempt to connect to an address until one succeeds
    for (struct addrinfo* ptr = addrresult; ptr != NULL; ptr = ptr->ai_next) {

      // Create a SOCKET for connecting to server
      socket_ = socket(ptr->ai_family, ptr->ai_socktype,
                             ptr->ai_protocol);

      if (socket_ == INVALID_SOCKET) {
        continue;
      }

      // Connect to server.
      status = ::connect( socket_, ptr->ai_addr, (int)ptr->ai_addrlen);
      if (status != SOCKET_ERROR) {
        break;
      }

      // failed to connect, close socket
      closesocket(socket_);
      socket_ = INVALID_SOCKET;
    }
    freeaddrinfo(addrresult);

    if (socket_ == INVALID_SOCKET) {
      cpen333::error(std::string("Unable to connect to server: ")
                         + server_ + std::string(":") + strport);
      return false;
    }

    open_ = true;
    connected_ = true;
    return true;
  }

  bool send(const std::string str) {
    return send(str.c_str(), str.length());
  }

  bool send(const char* buff, size_t len) {

    if (!connected_) {
      return false;
    }

    int status = ::send( socket_, buff, len, 0 );
    if (status == SOCKET_ERROR) {
      cpen333::error(std::string("send(...) failed with error: ")
                         + std::to_string(WSAGetLastError()));
      return false;
    }
    return true;
  }

  int receive(char* buff, int len) {

    if (!open_) {
      return -1;
    }

    int result = recv(socket_, buff, len, 0);
    if (result < 0) {
      cpen333::error(std::string("recv(...) failed with error: ")
                         + std::to_string(WSAGetLastError()));
    }
    return result;
  }

  bool disconnect() {

    if (!connected_) {
      return false;
    }

    // shutdown the connection since no more data will be sent
    int result = ::shutdown(socket_, SD_SEND);
    if (result == SOCKET_ERROR) {
      cpen333::error(std::string("shutdown(...) failed with error: ")
                         + std::to_string(WSAGetLastError()));
      return false;
    }
    connected_ = false;
    return true;
  }

  bool close() {

    if (!open_) {
      return false;
    }
    if (connected_) {
      disconnect();
    }

    // Receive and discard until the peer closes the connection
    static const int recvbuflen = 1024;
    char recvbuf[recvbuflen];
    int result = 0;
    do {
      result = recv(socket_, recvbuf, recvbuflen, 0);
      if (result < 0) {
        cpen333::error(std::string("recv failed with error: ")
                           + std::to_string(WSAGetLastError()));
      }
    } while( result > 0 );

    // cleanup
    closesocket(socket_);
    socket_ = INVALID_SOCKET;
    open_ = false;
    return true;
  }

};


class socket_server {
  int port_;
  SOCKET socket_;
  bool open_;
  WSASingleton& wsa_;

 public:
  socket_server() : port_(CPEN333_SOCKET_DEFAULT_PORT), socket_(INVALID_SOCKET),
                    open_(false), wsa_(WSASingleton::instance()) {
    wsa_.acquire();
  }
  socket_server(int port) : port_(port), socket_(INVALID_SOCKET),
                            open_(false), wsa_(WSASingleton::instance()) {
    wsa_.acquire();
  }

  ~socket_server() {
    close();
    wsa_.release();
  }

  bool start() {

    if (open_){
      return false;
    }

    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    struct addrinfo *addrresult = NULL;
    std::string strport = std::to_string(port_);
    int status = getaddrinfo(NULL, strport.c_str(), &hints, &addrresult);
    if ( status != 0 ) {
      cpen333::error(std::string("getaddrinfo(...) failed with error: ")
                         + std::to_string(status));
      return false;
    }

    // Create a SOCKET for connecting to server
    socket_ = socket(addrresult->ai_family, addrresult->ai_socktype,
                     addrresult->ai_protocol);
    if (socket_ == INVALID_SOCKET) {
      cpen333::error(std::string("socket(...) failed with error: ")
                         + std::to_string(WSAGetLastError()));
      freeaddrinfo(addrresult);
      return false;
    }

    // Setup the TCP listening socket
    status = bind( socket_, addrresult->ai_addr, (int)addrresult->ai_addrlen);
    if (status == SOCKET_ERROR) {
      cpen333::error(std::string("bind(...) failed with error: ")
                         + std::to_string(WSAGetLastError()));
      freeaddrinfo(addrresult);
      closesocket(socket_);
      socket_ = INVALID_SOCKET;
      return false;
    }
    freeaddrinfo(addrresult);

    if (port_ == 0) {
      struct sockaddr_in sin;
      int addrlen = sizeof(sin);
      status = getsockname(socket_, (struct sockaddr *)&sin, &addrlen);
      if(status == 0 ) {
        port_ = ntohs(sin.sin_port);
      } else {
        cpen333::error(std::string("getsockname(...) failed with error: ")
                           + std::to_string(status));
      }
    }

    status = listen(socket_, SOMAXCONN);
    if (status == SOCKET_ERROR) {
      cpen333::error(std::string("listen(...) failed with error: ")
                         + std::to_string(WSAGetLastError()));
      closesocket(socket_);
      socket_ = INVALID_SOCKET;
      return false;
    }

    open_ = true;
    return true;
  }

  bool accept(socket_client& client) {
    if (!open_) {
      return false;
    }

    SOCKET client_socket = INVALID_SOCKET;

    // Accept a client socket
    client_socket = ::accept(socket_, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
      cpen333::error(std::string("accept(...) failed with error: ")
                         + std::to_string(WSAGetLastError()));
      return false;
    }

    client.close();
    client.__initialize("", -1, client_socket, true, true);

    return true;
  }

  bool close() {
    if (!open_) {
      return false;
    }

    closesocket(socket_);
    socket_ = INVALID_SOCKET;
    open_ = false;
    return true;
  }

  int get_port() {
    return port_;
  }

};

} // windows

typedef windows::socket_client socket_client;
typedef windows::socket_server socket_server;

} // process
} // cpen333

#endif