#include <iostream>
#include <thread>

#include <cpen333/process/socket.h>


void server(int& port) {
  cpen333::process::socket_server server(port);
  server.start();
  port = server.port();

  cpen333::process::socket client;
  server.accept(client);

  std::string msg = "Hello, world!";
  std::cout << "server sending: " << msg << std::endl;
  client.send(msg);

  client.close();
  server.close();
}

void client(const std::string& server, int& port) {
  // wait for port to be assigned
  while (port == 0) {
    std::this_thread::yield();
  }
  std::cout << "connecting to port: " << port << std::endl;
  cpen333::process::socket client(server, port);
  client.open();

  const int buffsize = 512;
  char buff[buffsize];
  int len = 0;
  do {
    len = client.receive(buff, buffsize);
    std::string msg(buff, 0, len);
    if (len > 0) {
      std::cout << "client received: " << msg << std::endl;
    }
  } while (len > 0);
  client.close();
}

int main() {

  int port = 0;
  std::thread svr(server, std::ref(port));
  std::thread cl(client, "localhost", std::ref(port));

  svr.join();
  cl.join();

  return 0;
}