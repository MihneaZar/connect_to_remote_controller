#ifndef TIMER_HPP

#define TIMER_HPP

#include "../controller_console/controller_console.hpp"
#include <chrono>

// returns current time in miliseconds
uint64_t current_time_ms();

// returns the amount of miliseconds that have passed since given time
uint64_t time_ms_since(uint64_t recorded_time);

#endif