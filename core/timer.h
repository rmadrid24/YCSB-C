//
//  timer.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/19/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_TIMER_H_
#define YCSB_C_TIMER_H_

#include <chrono>

namespace utils {

template <typename T>
class Timer {
 public:
  void Start() {
    time_ = Clock::now();
  }

  double End() {
    T span;
    Clock::time_point t = Clock::now();
    span = std::chrono::duration_cast<T>(t - time_);
    return span.count();
  }

 private:
  typedef std::chrono::high_resolution_clock Clock;

  Clock::time_point time_;
};

} // utils

#endif // YCSB_C_TIMER_H_

