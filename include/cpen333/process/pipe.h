#ifndef CPEN333_PROCESS_PIPE_H
#define CPEN333_PROCESS_PIPE_H

#define PIPE_NAME_SUFFIX "_pipe_"
#define PIPE_WRITE_SUFFIX "_pipe_w_"
#define PIPE_READ_SUFFIX "_pipe_r_"
#define PIPE_INFO_SUFFIX "_pipe_i_"
#define PIPE_INITIALIZED 0x18763023

#include "cpen333/process/impl/named_resource.h"
#include "cpen333/process/mutex.h"
#include "cpen333/process/semaphore.h"
#include "cpen333/process/shared_memory.h"

// simulated pipe using shared memory and semaphores
namespace cpen333 {
namespace process {

class pipe : public named_resource {

 public:
  pipe(const std::string& name, size_t size = 1024) :
      named_resource{name + std::string(PIPE_NAME_SUFFIX)},
      wmutex_{name + std::string(PIPE_WRITE_SUFFIX)},
      rmutex_{name + std::string(PIPE_READ_SUFFIX)},
      info_{name + std::string(PIPE_INFO_SUFFIX)},
      pipe_{name + std::string(PIPE_NAME_SUFFIX), size},
      producer_{name + std::string(PIPE_WRITE_SUFFIX), 0},
      consumer_{name + std::string(PIPE_READ_SUFFIX), size} {

    // initialize info
    std::lock_guard<decltype(wmutex_)> lock(wmutex_);
    if (info_->initialized != PIPE_INITIALIZED) {
      info_->size = size;
      info_->read = 0;
      info_->write = 0;
      info_->initialized = PIPE_INITIALIZED; // mark as initialized
    }
  }

  bool write(const void* data, size_t size) {
    uint8_t *ptr = (uint8_t *) data;

    for (size_t i = 0; i < size; ++i) {
      consumer_.wait();  // wait until there is space in the pipe
      std::lock_guard<decltype(wmutex_)> lock(wmutex_);

      // write next byte and advance write index

      pipe_[info_->write] = *ptr;
      ++ptr;

      // next byte location to write, wrapping around if need be
      if ((++(info_->write)) == info_->size) {
        info_->write = 0;
      }

      producer_.notify();  // byte available for read
    }

    return true;
  }

  template<typename T>
  bool write(const T& data) {
    return this->write<T>(&data);
  }

  template<typename T>
  bool write(const T* data) {
    return this->write((void*)data, sizeof(T));
  }

  bool read(void* data, size_t size) {
    uint8_t *ptr = (uint8_t *) data;

    for (size_t i = 0; i < size; ++i) {
      producer_.wait();  // wait until there is data in the pipe
      std::lock_guard<decltype(rmutex_)> lock(rmutex_);

      // read next byte and advance read index
      *ptr = pipe_[info_->read];
      // next byte location to read, wrapping around if need be
      ++ptr;  // advance ptr
      if ((++(info_->read)) == info_->size) {
        info_->read = 0;
      }
      consumer_.notify();  // byte available for writing
    }

    return true;
  }

  /**
   * Read a single byte
   * @return next byte in the stream
   */
  uint8_t read() {
    uint8_t byte;
    this->read(&byte, 1);
    return byte;
  }

  template<typename T>
  bool read(T* data) {
    return read((void*)data, sizeof(T));
  }

  size_t available() {
    // lock both read and write to get indices
    std::lock_guard<decltype(wmutex_)> wlock(wmutex_);
    std::lock_guard<decltype(rmutex_)> rlock(rmutex_);
    auto w = info_->write;
    auto r = info_->read;
    if (w < r) {
      return info_->size-r+w;
    }
    return w-r;
  }

  bool unlink() {
    bool b1 = wmutex_.unlink();
    bool b2 = rmutex_.unlink();
    bool b3 = info_.unlink();
    bool b4 = pipe_.unlink();
    bool b5 = producer_.unlink();
    bool b6 = consumer_.unlink();
    return b1 && b2 && b3 && b4 && b5 && b6;
  }

  static bool unlink(const std::string& name) {

    bool b1 = cpen333::process::mutex::unlink(name + std::string(PIPE_WRITE_SUFFIX));
    bool b2 = cpen333::process::mutex::unlink(name + std::string(PIPE_READ_SUFFIX));
    bool b3 = cpen333::process::shared_object<pipe_info>::unlink(name + std::string(PIPE_INFO_SUFFIX));
    bool b4 = cpen333::process::shared_memory::unlink(name + std::string(PIPE_NAME_SUFFIX));
    bool b5 = cpen333::process::semaphore::unlink(name + std::string(PIPE_WRITE_SUFFIX));
    bool b6 = cpen333::process::semaphore::unlink(name + std::string(PIPE_READ_SUFFIX));

    return b1 && b2 && b3 && b4 && b5 && b6;
  }

 private:
  struct pipe_info {
    int initialized;
    size_t read;
    size_t write;
    size_t size;
  };

  cpen333::process::mutex wmutex_;
  cpen333::process::mutex rmutex_;
  cpen333::process::shared_object<pipe_info> info_;
  cpen333::process::shared_memory pipe_;
  cpen333::process::semaphore producer_;
  cpen333::process::semaphore consumer_;

};

}
}

#endif //CPEN333_PROCESS_PIPE_H
