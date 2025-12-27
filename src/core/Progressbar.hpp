//
// Created by 長井亨 on 2019-01-31.
//

#ifndef PATHTRACING_PROGRESSBAR_HPP
#define PATHTRACING_PROGRESSBAR_HPP

#include <cmath>
#include <iostream>
#include <sys/ioctl.h>
#include <thread>
#include <zconf.h>

namespace nagato
{
class Progressbar
{
  public:
    explicit Progressbar(int max);

    void reset(int max);

    void update();

    void update(int prog);

    void printBar();

    void printPercent();

  private:
    float max;
    float progress;
    int columns;
};
} // namespace nagato

#endif // PATHTRACING_PROGRESSBAR_HPP
