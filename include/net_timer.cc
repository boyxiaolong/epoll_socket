#include "net_timer.h"

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "log.h"

net_timer* net_timer::ptimer_ = nullptr;
int64_t get_miliseconds_now() {
  struct timeval spec;
  gettimeofday(&spec, NULL);
  return spec.tv_sec * 1000 + spec.tv_usec / 1000;
}

int net_timer::add_timer(timer_callback fun, int interval) {
    int next_id = _gen_next_id();

    const int64_t cur_time = get_miliseconds_now();

    timer_data data;
    data.id_ = next_id;
    data.inteval_ = interval;
    data.fire_time_ = cur_time + interval;
    data.fun_ = fun;

    timer_queue_.push(data);

    LOG("id :%d", next_id);

    return next_id;
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

int net_timer::update() {
    const int64_t cur_time = get_miliseconds_now();

    LOG("size %d", timer_queue_.size());

    while (!timer_queue_.empty()) {
        timer_data data = timer_queue_.top();
        if (data.fire_time_ > cur_time) {
            break;
        }

        timer_queue_.pop();

        if (remove_ids_.find(data.id_) != remove_ids_.end()){
            continue;
        }

        data.fun_(cur_time);
    }

    return 0;
}

net_timer* net_timer::get_instance() {
    if (ptimer_ == nullptr) {
        ptimer_ = new net_timer();
    }

    return ptimer_;
}

int net_timer::remove_timer(int id) {
    remove_ids_.insert(id);
}