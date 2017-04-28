#ifndef Q10_COMMON_H
#define Q10_COMMON_H

#include <string>

enum MessageCode {
  NOTE, GOODBYE
};

struct MessageType {
  MessageCode code;
  char msg[20];     // must use fixed-size array since will be copied to shared memory
  char sender[20];

  // need a default constructor
  MessageType() : code{MessageCode::NOTE}, msg{}, sender{} {}

  // convenience constructor to use string
  MessageType(MessageCode code, std::string msg, std::string sender) : code(code) , msg{}, sender{} {
    // copy strings to msg/sender
    int i = 0;
    for (; i<sizeof(this->msg)-1 && i < msg.length(); ++i) {
      this->msg[i] = msg[i];
    }
    this->msg[i] = 0; // terminating zero

    for (i=0; i<sizeof(this->sender)-1 && i < sender.length(); ++i) {
      this->sender[i] = sender[i];
    }
    this->sender[i] = 0;  // terminating zero
  }
};

#endif //Q10_COMMON_H
