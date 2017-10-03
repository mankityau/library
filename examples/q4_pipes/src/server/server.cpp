#include <cpen333/process/pipe.h>
#include <iostream>
#include <thread>

void service(cpen333::process::pipe&& pipe, int i) {

  // read buffer
  static const int buffsize = 1024;
  char buff[buffsize+1];  // extra for terminating \0
  buff[buffsize] = 0;

  // keep reading until we get zero bytes back
  int nread = 0;

  std::cout << "pipe " << i << " started" << std::endl;
  while ( (nread = pipe.read(buff, buffsize)) > 0 ) {
    buff[nread] = 0; // terminating zero
    if (nread > 0) {
      std::cout << "pipe " << i << ": " << buff << std::endl;
    }
  }

  pipe.close();
  std::cout << "pipe " << i << " closed." << std::endl;

}

int main() {

  cpen333::process::pipe_server::unlink("q4_pipes_server");  // clean-up
  cpen333::process::pipe_server server("q4_pipes_server");
  server.start();

  // connect to 10 remote processes
  for (int i=0; i<10; ++i) {
    std::cout << "Waiting for pipe " << i << std::endl;
    cpen333::process::pipe client;
    if (server.accept(client)) {
      std::thread thread(service, std::move(client), i);
      thread.detach();
    }
  }

  server.close();

  return 0;
}