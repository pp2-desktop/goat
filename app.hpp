#include <condition_variable>
#include <mutex>


class app {
public:
  app();
  ~app();

  static app& get(){
    static app instance;
    return instance;
  }

  int mid;
  size_t max_spin_cnt;
  size_t spin_cnt;
  bool is_done;
  bool is_next;
  std::string uid;
  std::condition_variable wait4next;
private:

};
