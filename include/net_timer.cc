#include "net_timer.h"

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

int get_miliseconds_now() {
  struct timeval spec;
  gettimeofday(&spec, NULL);
  return spec.tv_sec * 1000 + spec.tv_usec / 1000;
}

int net_timer::add_timer(timer_callback fun, int interval) {
    int next_id = _gen_next_id();

    const int cur_time = get_miliseconds_now();

    timer_data data;
    data.id_ = next_id;
    data.inteval_ = interval;
    data.fire_time_ = cur_time + interval;
    data.fun_ = fun;
    data.is_repeated = true;

    timer_queue_.push(data);

    return 0;
}

int net_timer::get_min_interval() {
    if (timer_queue_.empty()) {
        return -1;
    }
    return timer_queue_.top().fire_time_ - get_miliseconds_now();
}

int net_timer::_gen_next_id() {
    ++id_;
    return id_;
}

int net_timer::run() {
    const int cur_time = get_miliseconds_now();

    std::vector<timer_data> next_vec;

    while (!timer_queue_.empty()) {
        timer_data data = timer_queue_.top();
        if (data.fire_time_ > cur_time) {
            break;
        }

        timer_queue_.pop();

        data.fun_(cur_time);

        if (data.is_repeated) {
            timer_data new_data = data;
            new_data.fire_time_ = cur_time + new_data.inteval_;
            new_data.id_ = _gen_next_id();

            next_vec.push_back(new_data);
        }
    }

    for (auto data : next_vec) {
        timer_queue_.push(data);
    }

    return 0;
}