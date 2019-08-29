#include "../inc/Mutex.h"

class Request
{
 public:
  void process() // __attribute__ ((noinline))
  {
    lmp_tool::MutexLockGuard lock(mutex_);
    print();
  }

  void print() const // __attribute__ ((noinline))
  {
    lmp_tool::MutexLockGuard lock(mutex_);
  }

 private:
  mutable lmp_tool::MutexLock mutex_;
};

int main()
{
  Request req;
  req.process();
}
