#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "common.h"
#include "cpen333/process/subprocess.h"
#include "cpen333/process/message_queue.h"
#include "cpen333/thread/timer.h"  // timer

struct PalInfo {
  std::string name;
  cpen333::process::subprocess*  process;
  cpen333::process::message_queue<MessageType>* mailbox;
};

int main() {

  std::string my_name = "Po";
  cpen333::process::message_queue<MessageType> my_mailbox(my_name);

  std::vector<std::string> friend_names = {"Tigress", "Master Shifu", "Viper", "Monkey", "Mantis", "Crane"};
  std::vector<PalInfo> friends;

  // create a bunch of friends and mailboxes
  for (const std::string& pal : friend_names) {
    friends.push_back(PalInfo{
                        pal,
                        new cpen333::process::subprocess({"./PenPal", pal}, true, true),
                        new cpen333::process::message_queue<MessageType>(pal)
                      });
  }

  // on each timer event, send mail to friends
  // create a message-blast lambda that "captures" the friends vector and my_name
  auto message_blast = [&friends, &my_name]() {
    std::cout << "Blasting message" << std::endl;
    for (PalInfo& pal : friends) {
      pal.mailbox->send( {MessageCode::NOTE, std::string("Hi ") +  pal.name, my_name} );
    }
  };
  // create timer to send messages
  cpen333::thread::timer<std::chrono::seconds> timer(std::chrono::seconds(2), message_blast);
  timer.start();

  // create separate thread to read my own messages
  auto message_reader = [&my_name, &my_mailbox]() {
    while(true) {
      MessageType message = my_mailbox.receive();

      // check for poison pill
      if (message.code == MessageCode::GOODBYE) {
        break;
      }

      std::cout << my_name << " recieved message from " << message.sender << ": " << message.msg << std::endl;
    }
  };
  std::thread read_thread(message_reader);

  // Take a nap here for a bit while things run
  std::this_thread::sleep_for(std::chrono::seconds(60));

  timer.stop();  // stop timer

  // send a goodbye to everyone
  for (PalInfo& pal : friends) {
    pal.mailbox->send({MessageCode::GOODBYE, " ... ", my_name});
    pal.process->join();    // wait for process to terminate
    pal.mailbox->unlink();  // close down mailbox
    delete pal.mailbox;
    delete pal.process;
  }

  // say goodbye to myself
  my_mailbox.send({MessageCode::GOODBYE, "", my_name});
  read_thread.join();
  my_mailbox.unlink();

  std::cout << "Goodbye." << std::endl;
  return 0;

}
