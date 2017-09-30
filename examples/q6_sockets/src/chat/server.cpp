#include <cpen333/process/socket.h>
#include <mutex>
#include <thread>
#include <iostream>
#include <cstring>

#include "chat.h"

void service(cpen333::process::socket&& socket) {

  // mutex for protecting print statements
  static std::mutex mutex;

  // read buffer
  static const int buffsize = 1024;
  char buff[buffsize+1];  // extra for terminating \0
  buff[buffsize] = 0;

  // client's name
  std::string name = "";

  int read;
  while ((read  = socket.receive(buff, buffsize)) > 0) {

    // command
    if (buff[0] == chat::CMD_HELLO) {
      read = socket.receive(buff, buffsize);
      if (read > 0) {
        // get name
        buff[read] = 0;
        name = buff;

        mutex.lock();
        std::cout << "  " << name << " has joined." << std::endl;
        mutex.unlock();
      }
    } else if (buff[0] == chat::CMD_MSG) {
      read = socket.receive(buff, buffsize);
      if (read > 0) {
        // get message
        buff[read] = 0;
        mutex.lock();
        std::cout << name << ": \t" << buff << std::endl;
        mutex.unlock();
      }
    } else if (buff[0] == chat::CMD_GOODBYE) {
      mutex.lock();
      std::cout  << "  " << name << " has left." << std::endl;
      mutex.unlock();
      break;
    }
  } // next command

}

int main() {

  // choose port
  cpen333::process::socket_server server(0);
  server.start();

  int port = server.port();
  std::vector<std::string> address = server.address_lookup();
  std::cout << "List of addresses: " << std::endl;
  for (const auto& add : address) {
    std::cout << "  " << add << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Address: " << address.back() << std::endl;
  std::cout << "Port:    " << port << std::endl;
  std::cout << "Server is running..." << std::endl << std::endl;

  while(true) {
    cpen333::process::socket client;
    if (server.accept(client)) {
      std::thread thread(service, std::move(client));
      thread.detach();
    }
  }

  return 0;
}