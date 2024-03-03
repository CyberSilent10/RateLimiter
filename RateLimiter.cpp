#include <atomic>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <chrono>
#include "CircularArray.cpp"

using namespace std::chrono;

namespace RateLimiter {
  enum class Algo {
    FIXED_WINDOW_COUNTER = 0,
    SLIDING_WINDOW = 1
  };

  class RateLimiter {
  protected:
    int m_time_window;
    int m_limit;
  public:
    RateLimiter(int time_window, int limit) : m_time_window(time_window), m_limit(limit) {
    }
    virtual bool shouldAccept() = 0;
  };

  class FixedWindowCounter : public RateLimiter{
    int counter;
    std::thread th;
    std::mutex mt;
    void resetCounter() {
      sleep(m_time_window);
      counter = 0;
    }
  public:
    FixedWindowCounter(int time_window, int limit) : RateLimiter(time_window, limit) {
      th = std::thread(&FixedWindowCounter::resetCounter, this);
      th.detach();
    }
    // Copy constructor and copy assignment operator needs to be deleted because this 
    FixedWindowCounter(const FixedWindowCounter& obj) = delete;
    FixedWindowCounter& operator=(const FixedWindowCounter& obj) = delete;
    
    virtual bool shouldAccept() {
      std::unique_lock<std::mutex> ul{mt};
      if(counter >= m_limit) return false;
      counter++;
      return true;
    }
  };

  class SlidingWindow : public RateLimiter {
    CircularArray<uint64_t> arr;
    public:
      SlidingWindow(int time_window, int limit) : RateLimiter(time_window, limit), arr(m_limit) {}
      virtual bool shouldAccept() {
        uint64_t cur = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        uint64_t oldest = arr.getStart();
        if(cur - oldest <= m_time_window) {
          arr.push_back(cur);
          return true;
        }
        return false;
      }
  };
};
