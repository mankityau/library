#ifndef CPEN333_PROCESS_MESSAGE_QUEUE_H
#define CPEN333_PROCESS_MESSAGE_QUEUE_H

#define MESSAGE_QUEUE_SUFFIX "_mq_"

#include <string>
#include <chrono>
#include "cpen333/process/fifo.h"

namespace cpen333 {
namespace process {

// Basic implementation based on FIFO, does not support priorities or filtering
template<typename MessageType>
class message_queue : named_resource {
 public:
  using message_type = MessageType;

  message_queue(const std::string& name, size_t size = 1024) :
      named_resource{name + std::string(MESSAGE_QUEUE_SUFFIX)},
      fifo_{name + std::string(MESSAGE_QUEUE_SUFFIX), size} {}

  void send(const MessageType& msg) {
    fifo_.push(msg);
  }

  bool try_send(const MessageType& msg) {
    return fifo_.try_push(msg);
  }

  template <typename Rep, typename Period>
  bool try_send_for(const MessageType& val, std::chrono::duration<Rep, Period>& rel_time) {
    return try_send_until(val, std::chrono::steady_clock::now()+rel_time);
  }

  template<typename Clock, typename Duration>
  bool try_send_until(const MessageType& val, const std::chrono::time_point<Clock,Duration>& timeout) {
    return fifo_.try_push_until(val, timeout);
  }

  MessageType receive() {
    return fifo_.pop();
  }

  void receive(MessageType* out) {
    fifo_.pop(out);
  }

  bool try_receive(MessageType* out) {
    return fifo_.try_pop(out);
  }

  template <typename Rep, typename Period>
  bool try_receive_for(MessageType* val, std::chrono::duration<Rep, Period>& rel_time) {
    return try_receive_until(val, std::chrono::steady_clock::now()+rel_time);
  }

  template<typename Clock, typename Duration>
  bool try_receive_until(MessageType* val, const std::chrono::time_point<Clock,Duration>& timeout) {
    return fifo_.try_pop_until(val, timeout);
  }

  MessageType peek() {
    return fifo_.peek();
  }

  void peek(MessageType* out) {
    fifo_.peek(out);
  }

  bool try_peek(MessageType* out) {
    return fifo_.try_peek(out);
  }

  template <typename Rep, typename Period>
  bool try_peek_for(MessageType* val, std::chrono::duration<Rep, Period>& rel_time) {
    return try_peek_until(val, std::chrono::steady_clock::now()+rel_time);
  }

  template<typename Clock, typename Duration>
  bool try_peek_until(MessageType* val, const std::chrono::time_point<Clock,Duration>& timeout) {
    return fifo_.try_peek_until(val, timeout);
  }

  bool empty() {
    return fifo_.empty();
  }

  bool unlink() {
    return fifo_.unlink();
  }

  static bool unlink(const std::string& name) {
    return cpen333::process::fifo<MessageType>::unlink(name);
  }

 private:
  cpen333::process::fifo<MessageType> fifo_;

};

} // process
} // namespace

#endif //CPEN333_PROCESS_MESSAGE_QUEUE_H
