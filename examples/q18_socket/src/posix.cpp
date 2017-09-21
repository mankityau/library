#include <iostream>
#include <thread>

#include <cpen333/process/impl/posix/socket.h>

volatile int port;

void server() {
  cpen333::process::socket_server server(0);
  server.start();
  port = server.get_port();

  cpen333::process::socket_client client;
  server.accept(client);

  client.send("hello world");
  client.close();

  server.close();
}

void client() {
  // wait for port to be assigned
  while (port == 0) {
    std::this_thread::yield();
  }
  std::cout << "connecting to port: " << port << std::endl;
  cpen333::process::socket_client client("localhost", port);
  client.open();

  char buff[512];
  int len = 0;
  do {
    len = client.receive(buff, 512);
    std::string msg(buff, 0, len);
    if (len > 0) {
      std::cout << "client received: " << msg << std::endl;
    }
  } while (len > 0);
  client.close();
}

int main() {

  std::thread svr(server);
  std::thread cl(client);

  svr.join();
  cl.join();

  return 0;
}