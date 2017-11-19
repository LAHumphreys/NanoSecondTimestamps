//
// Created by lhumphreys on 19/11/17.
//
#include <iostream>
#include <chrono>
#include <iomanip>
#include <util_time.h>
#include <vector>

using namespace nstimestamp;

namespace {
    void report(const std::string& test,
                const std::chrono::high_resolution_clock::time_point& start,
                const std::chrono::high_resolution_clock::time_point& end,
                size_t itters)
    {
        long count = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        std::cout << std::left << std::setw(35) << test << ": " << std::setw(10) << count << "ns"
                  << " (" <<  double(count) / itters << "ns/itter)" << std::endl;
    }
}

#define BENCHMARK(name, code, itters) \
    { \
    auto start = std::chrono::high_resolution_clock::now(); \
    code \
    auto end = std::chrono::high_resolution_clock::now(); \
    report(name, start, end, itters); \
    }

namespace TimeBench {
    void WriteTimestamp() {
        const uint_fast32_t numEvents = 1e6;
        BENCHMARK("Timestamp creation", {
            std::string theTime;
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                theTime = Time().Timestamp();
            }
        }, numEvents);
    }

    void WriteISOTimestamp() {
        const uint_fast32_t numEvents = 1e6;
        BENCHMARK("ISO Timestamp creation", {
            std::string theTime;
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                theTime = Time().ISO8601Timestamp();
            }
        }, numEvents);
    }

    void ReadTimestamp() {
        const uint_fast32_t numEvents = 1e6;
        BENCHMARK("Timestamp parsing", {
            const std::string reftime = "20140403 10:11:02.294930000";
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                Time(reftime);
            }
        }, numEvents);
    }

    void ReadISOTimestamp() {
        const uint_fast32_t numEvents = 1e6;
        BENCHMARK("ISO Timestamp parsing", {
            const std::string reftime = "2014-04-03T10:11:02.294930Z";
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                Time(reftime);
            }
        }, numEvents);
    }

    void StackTime() {
        const uint_fast32_t numEvents = 1e6;
        BENCHMARK("Time - Stack temp", {
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                Time();
            }
        }, numEvents);
    }

    void EventCapture() {
        const uint_fast32_t numEvents = 1e6;
        std::vector<Time> events;
        events.reserve(numEvents);
        BENCHMARK("Time - Event Capture", {
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                events.emplace_back();
            }
        }, numEvents);
    }

    void PreAllocEventCapture() {
        const uint_fast32_t numEvents = 1e6;
        Time updateTime;
        BENCHMARK("Time - Event Capture (prealloc)", {
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                updateTime.SetNow();
            }
        }, numEvents);
    }
}

namespace ChronoBench {
    void StackTime() {
        const uint_fast32_t numEvents = 1e6;
        BENCHMARK("Chrono - Stack temp", {
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                std::chrono::system_clock::now();
            }
        }, numEvents);
    }
    void EventCapture() {
        const uint_fast32_t numEvents = 1e6;
        std::vector<std::chrono::system_clock::time_point> events;
        events.reserve(numEvents);
        BENCHMARK("Chrono - Event Capture", {
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                events.emplace_back(std::chrono::system_clock::now());
            }
        }, numEvents);
    }

    void PreAllocEventCapture() {
        const uint_fast32_t numEvents = 1e6;
        auto updateTime = std::chrono::system_clock::now();
        BENCHMARK("Chrono - Event Capture (prealloc)", {
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                updateTime = std::chrono::system_clock::now();
            }
        }, numEvents);
    }
}
namespace NakedTmBench {
    void EventCapture() {
        const uint_fast32_t numEvents = 1e6;
        timeval updateTime;
        BENCHMARK("gettimeofday - Event Capture", {
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                gettimeofday(&updateTime, NULL);
            }
        }, numEvents);
    }
}

namespace NakedTimeSpec {
    void EventCapture() {
        const uint_fast32_t numEvents = 1e6;
        timespec updateTime;
        BENCHMARK("clock_gettime - Event Capture", {
            for (uint_fast32_t i = 0; i < numEvents; ++i) {
                clock_gettime(CLOCK_REALTIME, &updateTime);
            }
        }, numEvents);
    }
}

int main(int argc, char**argv) {
    std::cout << "timeval size: " << sizeof(timeval) << std::endl;
    std::cout << "tm size: " << sizeof(tm) << std::endl;
    std::cout << "bool size   : " << sizeof(bool) << std::endl;
    std::cout << "Time size   : " << sizeof(Time) << std::endl;
    BENCHMARK("[COLD] Chrono - now", {
        std::chrono::system_clock::now();
    }, 1);

    BENCHMARK("[COLD] Time - now", {
        Time();
    }, 1);
    for (size_t i =0; i < 10000; ++i ) {
        std::chrono::system_clock::now();
    }
    for (size_t i =0; i < 10000; ++i ) {
        Time start;
    }
    BENCHMARK("[WARM] NO-OP", { }, 1);
    BENCHMARK("[WARM] Chrono - now", {
        std::chrono::system_clock::now();
    }, 1);
    BENCHMARK("[WARM] Time - now", {
        Time();
    }, 1);

    std::cout << std::endl;
    TimeBench::EventCapture();
    ChronoBench::EventCapture();

    std::cout << std::endl;
    TimeBench::PreAllocEventCapture();
    ChronoBench::PreAllocEventCapture();

    std::cout << std::endl;
    TimeBench::StackTime();
    ChronoBench::StackTime();

    std::cout << std::endl;
    TimeBench::WriteTimestamp();
    TimeBench::WriteISOTimestamp();

    std::cout << std::endl;
    TimeBench::ReadTimestamp();
    TimeBench::ReadISOTimestamp();

    std::cout << std::endl;
    NakedTmBench::EventCapture();
    NakedTimeSpec::EventCapture();

    return 0;
}
