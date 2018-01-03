# NanoSecondTimestamps
Capture, print and parse nanosecond UTC Timestmaps.

[![Build Status](https://travis-ci.org/Grauniad/NanoSecondTimestamps.svg?branch=master)](https://travis-ci.org/Grauniad/NanoSecondTimestamps)
[![Coverage Status](https://coveralls.io/repos/github/Grauniad/NanoSecondTimestamps/badge.svg?branch=Travis)](https://coveralls.io/github/Grauniad/NanoSecondTimestamps?branch=Travis)

## Overview
The nstimestamp utility provides a simplified wrapper around the Linux
clock_gettime call. With a simpler, less rich, interface than
std::chrono it can be used for:
  - Capturing nanosecond timestamps
  - Parsing string timestamps (including ISO8601 timestamps)
  - Calculating the elapsed time between two stamps
  - Reporting of timestamps (String formatting, as well component access: Year,
    Minute, nanosecond etc)

## Example: Timestamp capture
```c++
   Time eventTime;
   // ...
   eventTime.SetNow();
   // ...
   std::cout << "Event occurred: " << event.Timestamp() << std::endl;
```

## Example: Wall time calculation
```c++
   Time start;
   benchmark();
   Time stop;
   std::cout << "Duration: " << stop.DiffUs(start) << "us" << std::endl;
```

## Example: Parse timestamps
```c++
   Time reftime("2014-04-03T10:11:02.294930Z");
   std::cout << "Year: " << reftime.Year() << std::endl;
   std::cout << "Month: " << reftime.Month() << std::endl;
   std::cout << "Second: " << reftime.Second() << std::endl;
   std::cout << "Micro Second: " << reftime.USec() << std::endl;

   std::cout << "Seconds since the Epoch : " << reftime.EpochSecs << std::endl;
```

## Performance - Event Capture
Capturing the current time with SetNow() is entirely equivalent to a naked
clock_gettime call, and performs as such:

Benchmark                                     |  Time per itter
----------                                    | ---------------
timestamp.SetNow()                            | 20ns
updateTime = std::chrono::system_clock::now() | 20ns
gettimeofday(&updateTime, NULL)               | 18ns
clock_gettime(CLOCK_REALTIME, &updateTime)    | 18ns

(Benchmarks are run 1 million times in a tight loop. Reported time is total
time / 1e6)

As the Time class supports component level access, it has an additional struct
of data. This is not initialised until (unless) needed, but does lead to a
larger instance size. Consequently there is a slight impact to the construction
benchmark:

Benchmark                                             |  Time per itter
----------                                            | ---------------
timestamps.emplace_back()                             | 32ns
events.emplace_back(std::chrono::system_clock::now()) | 24ns

## Performance - Timestamp Parsing
The string parsing logic has been optimized to allow rapid parsing of valid UTC
timetamps.

Benchmark            |  Time per itter
----------           | ---------------
Time(isotimestamp)   | 19ns
Time(nstimestamp)    | 20ns


## Build Instructions
### Prerequisites
nstimestamp is built for and tested for Linux. Other systems exposing a
compatible clock_gettime method should also work.

Building and running the tests requires a modern installation of gtest.

### Build and install.

```sh
#
# Setup the build
#
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=<install path> .. .

#
# Build & Install
#
make
make install
```
### Linking with CMake
The default installation exports a UtilTime package, which can be used to add
the Time library as a dependency

```cmake
find_package(UtilTime 0.1 REQUIRED)

target_link_libraries(Log PRIVATE UtilTime::Time)
```

### Manually Linking against nstimestamp
The Time class is defined in util_time.h. The default installation will export
a libTime.a which should be added to the link time dependencies
