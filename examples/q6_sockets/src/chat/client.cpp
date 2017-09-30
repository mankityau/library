// prevent Windows from defining max macro
#include <cpen333/process/socket.h>
#include <limits>
#include "chat.h"

int main() {

  std::string server;
  int port;

  std::cout << "Enter address: ";
  std::cin >> server;
  std::cout << "Enter port:    ";
  std::cin >> port;
  std::cout << std::endl;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

  {
    cpen333::process::socket client(server, port);
    client.open();

    std::string msg;

    std::cout << "Connected to server." << std::endl << std::endl;

    // say hello to server
    std::cout << "Enter your name: ";
    std::getline(std::cin, msg);
    client.send(&chat::CMD_HELLO, 1);
    client.send(msg);

    std::cout << std::endl << "Enter \"quit\" to exit" << std::endl << std::endl;

    // send messages to server
    do {
      std::cout << "Enter message: ";
      std::getline(std::cin, msg);
      if (msg == "quit") {
        client.send(&chat::CMD_GOODBYE, 1);
        break;
      }
      client.send(&chat::CMD_MSG, 1);
      client.send(msg);
    } while (true);

    client.close();
  }

  std::cout << "client should quit" << std::endl;
  cpen333::pause();
  return 0;
}