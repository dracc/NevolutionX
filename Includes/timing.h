#ifndef NEVOLUTIONX_TIMING_H
#define NEVOLUTIONX_TIMING_H

#include <chrono>

long long millisSince(const std::chrono::steady_clock::time_point& ref);

long long millisBetween(const std::chrono::steady_clock::time_point& start,
                        const std::chrono::steady_clock::time_point& end);

#endif // NEVOLUTIONX_TIMING_H
