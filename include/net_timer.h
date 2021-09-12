#ifndef _net_timer_h__
#define _net_timer_h__

#include <queue>
#include <functional>

typedef std::function<void(int)> timer_callback;

struct timer_data {
    int id_;
    int fire_time_;
    int inteval_;
    bool is_repeated;

    timer_callback fun_;
    bool operator <(const timer_data& value) const{
        if (fire_time_ != value.fire_time_){
            return fire_time_ > value.fire_time_;
        }
        return id_ > value.id_;
    }
};

class net_timer {
public:
    int add_timer(timer_callback fun, int interval);

    int get_min_interval();

    int run();

private:
    int _gen_next_id();

private:
    int id_ = 0;
    std::priority_queue<timer_data> timer_queue_;
};

#endif
