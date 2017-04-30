#ifndef CPEN333_PROCESS_FIFO_H
#define CPEN333_PROCESS_FIFO_H

#define FIFO_SUFFIX "_fifo_"
#define FIFO_PRODUCER_SUFFIX "_fifo_p_"
#define FIFO_CONSUMER_SUFFIX "_fifo_c_"
#define FIFO_INITIALIZED 0x88372612

#include <string>
#include <chrono>

#include "cpen333/process/impl/named_resource.h"
#include "cpen333/process/shared_memory.h"
#include "cpen333/process/mutex.h"
#include "cpen333/process/semaphore.h"

namespace cpen333 {
namespace process {

/**
 * Simple multi-process first-in-first-out queue using a circular buffer.  Push will block until space is available in
 * the queue.  Pop will block until there is an item in the queue.
 * @tparam ValueType type of data to store in the queue
 */
template<typename ValueType>
class fifo : named_resource {

 public:
  using value_type = ValueType;

  fifo(const std::string& name, size_t size = 1024) :
      named_resource{name + std::string(FIFO_SUFFIX)},
      memory_{name + std::string(FIFO_SUFFIX), sizeof(fifo_info)+size*sizeof(ValueType)}, // reserve memory
      info_{nullptr}, data_{nullptr}, // will initialize these after memory valid
      pmutex_{name + std::string(FIFO_PRODUCER_SUFFIX)},
      cmutex_{name + std::string(FIFO_CONSUMER_SUFFIX)},
      psem_{name + std::string(FIFO_PRODUCER_SUFFIX), size},  // start at size of fifo
      csem_{name + std::string(FIFO_CONSUMER_SUFFIX), 0} {    // start at zero

    // info is at start of memory block, followed by the actual data in the fifo
    info_ = (fifo_info*)memory_.get();
    data_ = (ValueType*)memory_.get(sizeof(fifo_info));  // start of fifo data is after fifo info

    // protect memory with a mutex (either one) to check if data is initialized, and initialize if not
    // This is only to stop multiple constructors from simultaneously trying to initialize data
    std::lock_guard<cpen333::process::mutex> lock(pmutex_);
    if (info_->initialized != FIFO_INITIALIZED) {
      info_->pidx = 0;
      info_->cidx = 0;
      info_->size = size;
      info_->initialized = FIFO_INITIALIZED;  // mark initialized
    }
  }

  fifo(fifo&& other) : memory_{std::move(other.memory_)}, info_{nullptr}, data_{nullptr}, // will initialize these after memory valid
                               pmutex_{std::move(other.pmutex_)},
                               cmutex_{std::move(other.cmutex_)},
                               psem_{std::move(other.psem_)},  // start at size of fifo
                               csem_{std::move(other.csem_)} {
    // info is at start of memory block, followed by the actual data in the fifo
    info_ = (fifo_info*)memory_.get();
    data_ = (ValueType*)memory_.get(sizeof(fifo_info));  // start of fifo data is after fifo info
    // should already be initialized since "other" already existed
  }

  /**
   * Add a item to the fifo
   * @param val
   */
  void push(const ValueType &val) {
    psem_.wait();   // wait until room to push
    push_item(val);
    csem_.notify(); // let consumer know a item is available

  }

  bool try_push(const ValueType &val) {
    // see if room to push
    if (!psem_.try_wait()) {
      return false;
    }
    push_item(val);
    csem_.notify();  // let consumer know a item is available
    return true;
  }
  
  template <typename Rep, typename Period>
  bool try_push_for(const ValueType& val, std::chrono::duration<Rep, Period>& rel_time) {
    return try_push_until(val, std::chrono::steady_clock::now()+rel_time);
  }

  template<typename Clock, typename Duration>
  bool try_push_until(const ValueType& val, const std::chrono::time_point<Clock,Duration>& timeout) {
    // wait until room to push
    if (!psem_.wait_until(timeout)) {
      return false;
    }
    push_item(val);
    csem_.notify();  // let consumer know a item is available
    return true;
  }

  /**
   * Populates memory pointed to by "out" with next item in fifo
   * @param out destination.  If nullptr, item is removed but not returned.
   */
  void pop(ValueType* out) {
    csem_.wait();      // wait until item available
    pop_item(out);
    psem_.notify();    // let producer know that we are done with the slot
  }
  
  ValueType pop() {
    ValueType out;
    pop(&out);
    return out;
  }
  
  bool try_pop(ValueType* out) {
    // see if data to pop
    if (!csem_.try_wait()) {
      return false;
    } 
    pop_item(out);
    psem_.notify();    // let producer know that we are done with the slot
    return true;
  }

  template <typename Rep, typename Period>
  bool try_pop_for(ValueType* val, std::chrono::duration<Rep, Period>& rel_time) {
    return try_pop_until(val, std::chrono::steady_clock::now()+rel_time);
  }

  template<typename Clock, typename Duration>
  bool try_pop_until(ValueType* val, const std::chrono::time_point<Clock,Duration>& timeout) {
    // wait until room to push
    if (!csem_.wait_until(timeout)) {
      return false;
    }
    pop_item(val);
    psem_.notify();  // let consumer know a item is available
    return true;
  }

  /**
   * Populates memory pointed to by "out" with next item in fifo, blocking if no items
   * @param out destination.  If nullptr, nothing happens.
   */
  void peek(ValueType* out) {
    csem_.wait();      // wait until item available
    peek_item(out);
  }

  ValueType peek() {
    ValueType out;
    peek(&out);
    return out;
  }

  bool try_peek(ValueType* out) {
    // see if data to peek
    if (!csem_.try_wait()) {
      return false;
    }
    peek_item(out);
    return true;
  }

  template <typename Rep, typename Period>
  bool try_peek_for(ValueType* val, std::chrono::duration<Rep, Period>& rel_time) {
    return try_peek_until(val, std::chrono::steady_clock::now()+rel_time);
  }

  template<typename Clock, typename Duration>
  bool try_peek_until(ValueType* val, const std::chrono::time_point<Clock,Duration>& timeout) {
    // wait until room to push
    if (!csem_.wait_until(timeout)) {
      return false;
    }
    peek_item(val);
    return true;
  }

  /**
   * Number of items in the fifo
   * @return number of items
   */
  size_t size() {
    std::lock_guard<cpen333::process::mutex> lock1(pmutex_);
    std::lock_guard<cpen333::process::mutex> lock2(cmutex_);

    if (info_->pidx < info_->cidx) {
      return info_->size - info_->cidx+info_->pidx;
    }
    return info_->pidx-info_->cidx;
  }

  /**
   * Check if fifo queue is empty
   * @return true if empty
   */
  bool empty() {
    std::lock_guard<cpen333::process::mutex> lock1(pmutex_);
    std::lock_guard<cpen333::process::mutex> lock2(cmutex_);
    return info_->pidx == info_->cidx;
  }

  /**
   * Detach names from all internal resources
   */
  bool unlink() {
    bool b1 = memory_.unlink();
    bool b2 = pmutex_.unlink();
    bool b3 = cmutex_.unlink();
    bool b4 = psem_.unlink();
    bool b5 = csem_.unlink();
    return b1 && b2 && b3 && b4 && b5;
  }

  /**
   * Detach names from all internal resources
   * @param name name of the item fifo
   */
  static bool unlink(const std::string& name) {
    bool b1 = cpen333::process::shared_memory::unlink(name + std::string(FIFO_SUFFIX));
    bool b2 = cpen333::process::mutex::unlink(name + std::string(FIFO_PRODUCER_SUFFIX));
    bool b3 = cpen333::process::mutex::unlink(name + std::string(FIFO_CONSUMER_SUFFIX));
    bool b4 = cpen333::process::semaphore::unlink(name + std::string(FIFO_PRODUCER_SUFFIX));
    bool b5 = cpen333::process::semaphore::unlink(name + std::string(FIFO_CONSUMER_SUFFIX));
    return b1 && b2 && b3 && b4 && b5;
  }

 private:

  // only to be called internally, does not wait for semaphore
  void push_item(const ValueType &val) {
    // lock is fast (only index increment), so will not trigger wait
    size_t loc = 0;
    {
      // look at index, protect memory from multiple simultaneous pushes
      std::lock_guard<cpen333::process::mutex> lock(pmutex_);
      loc = info_->pidx;
      // increment producer index for next item, wrap around if at end
      if ((++info_->pidx) == info_->size) {
        info_->pidx = 0;
      }
      // lock will unlock here as guard runs out of scope
    }

    // copy data to correct location
    data_[loc] = val;  // add item to fifo

  }

  void peek_item(ValueType* val) {
    // lock is fast (only index increment), so will not trigger wait
    size_t loc = 0;  // will store location of item to take
    {
      // look at index, protect memory from multiple simultaneous pops
      std::lock_guard<cpen333::process::mutex> lock(cmutex_);
      loc = info_->cidx;
      // lock will unlock here as guard runs out of scope
    }

    // copy data to output
    if (val != nullptr) {
      *val = data_[loc];  // copy item
    }
  }

  void pop_item(ValueType* val) {
    // lock is fast (only index increment), so will not trigger wait
    size_t loc = 0;  // will store location of item to take
    {
      // look at index, protect memory from multiple simultaneous pops
      std::lock_guard<cpen333::process::mutex> lock(cmutex_);
      loc = info_->cidx;

      // increment consumer index for next item, wrap around if at end
      if ( (++info_->cidx) == info_->size) {
        info_->cidx = 0;
      }
      // lock will unlock here as guard runs out of scope
    }

    // copy data to output
    if (val != nullptr) {
      *val = data_[loc];  // copy item
    }
  }

  struct fifo_info {
    size_t pidx;      // producer index
    size_t cidx;      // consumer index
    size_t size;      // size (in counts of ValueType)
    size_t initialized;  // magic initialized marker
  };

  cpen333::process::shared_memory memory_;   // actual memory
  fifo_info* info_;                         // pointer to fifo information, will be at start of memory_
  ValueType* data_;                        // pointer to data in fifo, after info_ in memory
  cpen333::process::mutex pmutex_;           // mutex for protecting memory modified by producers
  cpen333::process::mutex cmutex_;           // mutex for protecting memory modified by consumers
  cpen333::process::semaphore psem_;         // semaphore controlling when producer can add an item
  cpen333::process::semaphore csem_;         //     "            "         consumer can remove an item

};

} // process
} // cpen333

#endif //CPEN333_PROCESS_FIFO_H
