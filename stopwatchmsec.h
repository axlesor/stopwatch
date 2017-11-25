#pragma once

#ifndef PERF_STOPWATCH_MILLISEC_H
#define PERF_STOPWATCH_MILLISEC_H

#include <chrono>
#include "stopwatch.h"


typedef basic_stopwatch< TimerBaseChrono< std::chrono::system_clock, std::chrono::milliseconds> > Stopwatch;

# endif
