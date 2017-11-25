#pragma once

#ifndef PERF_STOPWATCH_MICROSEC_H
#define PERF_STOPWATCH_MICROSEC_H

#include <chrono>
#include "stopwatch.h"


typedef basic_stopwatch< TimerBaseChrono< std::chrono::high_resolution_clock, std::chrono::microseconds> > Stopwatchmicro;

# endif
