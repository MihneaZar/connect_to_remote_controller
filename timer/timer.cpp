#include "timer.hpp"

uint64_t current_time_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

// returns the amount of miliseconds that have passed since given time
uint64_t time_ms_since(uint64_t recorded_time) {
    return current_time_ms() - recorded_time;
}