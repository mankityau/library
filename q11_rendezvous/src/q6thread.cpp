#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

// Inter-thread rendezvous class
class rendezvous {
  std::mutex mutex_;
  std::condition_variable cv_;
  size_t countdown_;
  size_t countup_;
  size_t size_;

 public:
  rendezvous(size_t size) : mutex_{}, countdown_{size}, countup_{0}, size_{size} {}

  // Cause threads to wait until all arrive
  void wait() {
    // lock to protect access to data
    std::unique_lock<std::mutex> lock(mutex_);

    //check if we are done
    if (countdown_ <= 1) {
      // release all threads
      countdown_ = 0;
      cv_.notify_all();
    } else {
      --countdown_;

      // wait until everybody has arrived
      // We use a simple lambda to check condition
      cv_.wait(lock, [&](){return countdown_ == 0;});
    }

    // reset count upwards
    ++countup_;
    // when last thread exists method, reset countdown_
    if (countup_ == size_) {
      countdown_ = size_;
      countup_ = 0;
    }
  }
};

// Thread method
void ThreadFunction(int id, rendezvous& rdv) {

  std::cout << "Thread " << id << " running ...." << std::endl;

  for(int i = 0; i < 10; ++i)	{
    std::this_thread::sleep_for(std::chrono::seconds(id));  // sleep for id seconds
    std::cout << "Thread " << id << " Arriving at Rendezvous....." << std::endl;
    rdv.wait();                                             // wait at the rendezvous point
    std::cout << "Go " << id << std::endl;
  }
}

int main() {

  // create 6 threads
  std::thread *threads[6];

  rendezvous rdv{6};
  for (int i=0; i<6; ++i) {
    std::cout << "Creating thread " << i << std::endl;
    // pass integer id and rendezvous to thread function
    threads[i] = new std::thread(ThreadFunction, i+1, std::ref(rdv));
  }

  // join all threads
  for (int i=0; i<6; ++i) {
    threads[i]->join();
    delete threads[i];
  }

  return 0;
}



