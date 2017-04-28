#ifndef CPEN333_THREAD_FIFO_H
#define CPEN333_THREAD_FIFO_H

#include <string>
#include <chrono>

#include <mutex>
#include "cpen333/thread/semaphore.h"

namespace cpen333 {
namespace thread {

/**
 * Simple multi-thread first-in-first-out queue using a circular buffer.  Push will block until space is available in
 * the queue.  Pop will block until there is an item in the queue.
 * @tparam ValueType type of data to store in the queue
 */
template<typename ValueType = unsigned long>
class fifo {

 public:
  using value_type = ValueType;

  fifo(size_t size = 1024) :
      info_{0, 0, size}, data_{nullptr}, // will initialize later
      pmutex_{}, cmutex_{},
      psem_{size},  // start at size of fifo
      csem_{0} {    // start at zero
    data_ = new ValueType[size];
  }
  
  ~fifo() {
    // free data
    delete [] data;
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

  /**
   * Add a item to the fifo
   * @param val
   */
  void push(ValueType &&val) {
    psem_.wait();   // wait until room to push
    push_item(std::move(val));
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
  };

  template<typename Clock, typename Duration>
  bool try_push_until(const ValueType& val, const std::chrono::time_point<Clock,Duration>& timeout) {
    // wait until room to push
    if (!psem_.wait_until(timeout)) {
      return false;
    }
    push_item(val);
    csem_.notify();  // let consumer know a item is available
    return true;
  };

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
  
  bool try_pop(ValueType* val) {
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
  };

  template<typename Clock, typename Duration>
  bool try_pop_until(ValueType* val, const std::chrono::time_point<Clock,Duration>& timeout) {
    // wait until room to push
    if (!csem_.wait_until(timeout)) {
      return false;
    }
    pop_item(val);
    psem_.notify();  // let consumer know a item is available
    return true;
  };

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

  bool try_peek(ValueType* val) {
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
  };

  template<typename Clock, typename Duration>
  bool try_peek_until(ValueType* val, const std::chrono::time_point<Clock,Duration>& timeout) {
    // wait until room to push
    if (!csem_.wait_until(timeout)) {
      return false;
    }
    peek_item(val);
    return true;
  };

  /**
   * Number of items in the fifo
   * @return number of items
   */
  size_t size() {
    std::lock_guard<std::mutex> lock1(pmutex_);
    std::lock_guard<std::mutex> lock2(cmutex_);

    if (info_.pidx < info_.cidx) {
      return info_.size - info_.cidx+info_.pidx;
    }
    return info_.pidx-info_.cidx;
  }

  /**
   * Check if fifo queue is empty
   * @return true if empty
   */

 private:

  // only to be called internally, does not wait for semaphore
  void push_item(const ValueType &val) {
    // lock is fast (only index increment), so will not trigger wait
    int loc = 0;
    {
      // look at index, protect memory from multiple simultaneous pushes
      std::lock_guard<std::mutex> lock(pmutex_);
      loc = info_.pidx;
      // increment producer index for next item, wrap around if at end
      if ((++info_.pidx) == info_.size) {
        info_.pidx = 0;
      }
      // lock will unlock here as guard runs out of scope
    }

    // copy data to correct location
    data_[loc] = val;  // add item to fifo
  }

  // only to be called internally, does not wait for semaphore
  void push_item(ValueType &&val) {
    // lock is fast (only index increment), so will not trigger wait
    int loc = 0;
    {
      // look at index, protect memory from multiple simultaneous pushes
      std::lock_guard<std::mutex> lock(pmutex_);
      loc = info_.pidx;
      // increment producer index for next item, wrap around if at end
      if ((++info_.pidx) == info_.size) {
        info_.pidx = 0;
      }
      // lock will unlock here as guard runs out of scope
    }

    // copy data to correct location
    data_[loc] = std::move(val);  // add item to fifo
  }

  void peek_item(ValueType* val) {
    // lock is fast (only index increment), so will not trigger wait
    int loc = 0;  // will store location of item to take
    {
      // look at index, protect memory from multiple simultaneous pops
      std::lock_guard<std::mutex> lock(cmutex_);
      loc = info_.cidx;
      // lock will unlock here as guard runs out of scope
    }

    // copy data to output
    if (val != nullptr) {
      *val = data_[loc];  // copy item out
    }
  }

  void pop_item(ValueType* val) {
    // lock is fast (only index increment), so will not trigger wait
    int loc = 0;  // will store location of item to take
    {
      // look at index, protect memory from multiple simultaneous pops
      std::lock_guard<std::mutex> lock(cmutex_);
      loc = info_.cidx;

      // increment consumer index for next item, wrap around if at end
      if ( (++info_.cidx) == info_.size) {
        info_.cidx = 0;
      }
      // lock will unlock here as guard runs out of scope
    }

    // copy data to output
    if (val != nullptr) {
      *val = std::move(data_[loc]);  // move item out
    }
  }

  struct fifo_info {
    size_t pidx;      // producer index
    size_t cidx;      // consumer index
    size_t size;      // size (in counts of ValueType)
    int initialized;  // magic initialized marker
  };

  fifo_info info_;                          // pointer to fifo information, will be at start of memory_
  ValueType* data_;                         // pointer to data in fifo, after info_ in memory
  std::mutex pmutex_;                       // mutex for protecting memory modified by producers
  std::mutex cmutex_;                       // mutex for protecting memory modified by consumers
  cpen333::thread::semaphore psem_;         // semaphore controlling when producer can add an item
  cpen333::thread::semaphore csem_;         //     "            "         consumer can remove an item

};

} // thread
} // cpen333

#endif //CPEN333_THREAD_FIFO_H
