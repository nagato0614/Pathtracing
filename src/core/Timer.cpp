//
// Created by 長井亨 on 2019/01/30.
//

#include "core/Timer.hpp"

namespace nagato
{
Timer::Timer() {}

void Timer::start() { startTime = sys::now(); }

void Timer::stop() { endTime = sys::now(); }
} // namespace nagato
