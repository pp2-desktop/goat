#include <limits>
#include <cstddef>
#include <iostream>
#include "app.hpp"

app::app() {
  mid = -1;
  max_spin_cnt = std::numeric_limits<size_t>::max();
  spin_cnt = 0;
  is_done = false;
}

app::~app() {

}
