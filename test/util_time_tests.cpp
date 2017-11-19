#include <gtest/gtest.h>
#include <util_time.h>
#include <chrono>

using namespace std;

namespace {
    const string reftime = "20140403 10:11:02.294930";
    const string reftime_iso8601 = "2014-04-03T10:11:02.294930Z";

    void AssertTimeMatches(const Time& timeToCheck) {
        ASSERT_EQ(timeToCheck.Year(), 2014);
        ASSERT_EQ(timeToCheck.Month(), 4);
        ASSERT_EQ(timeToCheck.MDay(), 3);
        ASSERT_EQ(timeToCheck.Hour(), 10);
        ASSERT_EQ(timeToCheck.Minute(), 11);
        ASSERT_EQ(timeToCheck.Second(), 2);
        ASSERT_EQ(timeToCheck.MSec(), 294);
        ASSERT_EQ(timeToCheck.USec(), 294930);
        ASSERT_EQ(timeToCheck.Timestamp(), reftime);
        ASSERT_EQ(timeToCheck.ISO8601Timestamp(), reftime_iso8601);
    }

    void AssertIsEpoch(const Time& timeToCheck) {
        ASSERT_EQ(timeToCheck.Year(), 1970);
        ASSERT_EQ(timeToCheck.Month(), 1);
        ASSERT_EQ(timeToCheck.MDay(), 1);
        ASSERT_EQ(timeToCheck.Hour(), 0);
        ASSERT_EQ(timeToCheck.Minute(), 0);
        ASSERT_EQ(timeToCheck.Second(), 0);
        ASSERT_EQ(timeToCheck.MSec(), 0);
        ASSERT_EQ(timeToCheck.USec(), 0);
        ASSERT_EQ(timeToCheck.Timestamp(), Time::EpochTimestamp);
        ASSERT_EQ(timeToCheck.EpochSecs(), 0);
        ASSERT_EQ(timeToCheck.EpochUSecs(), 0);
    }
}

TEST(Timestamps,NowSecs) {
    auto start = std::chrono::system_clock::now();
    Time now;
    auto end = std::chrono::system_clock::now();
    long startEpochSecs = std::chrono::duration_cast<std::chrono::seconds>(
            start.time_since_epoch()).count();
    long endEpochSecs = std::chrono::duration_cast<std::chrono::seconds>(
            end.time_since_epoch()).count();

    ASSERT_GE(now.EpochSecs(), startEpochSecs);
    ASSERT_LE(now.EpochSecs(), endEpochSecs);
}

TEST(Timestamps,NowUSecs) {
    auto start = std::chrono::system_clock::now();
    Time now;
    auto end = std::chrono::system_clock::now();
    long startEpochUSecs = std::chrono::duration_cast<std::chrono::microseconds>(
            start.time_since_epoch()).count();
    long endEpochUSecs = std::chrono::duration_cast<std::chrono::microseconds>(
            end.time_since_epoch()).count();

    ASSERT_GE(now.EpochUSecs(), startEpochUSecs);
    ASSERT_LE(now.EpochUSecs(), endEpochUSecs);
}

TEST(Timestamps, SetNow) {
    Time timestamp(reftime);
    AssertTimeMatches(timestamp);
    auto start = std::chrono::system_clock::now();
    timestamp.SetNow();
    auto end = std::chrono::system_clock::now();
    long startEpochSecs = std::chrono::duration_cast<std::chrono::seconds>(
            start.time_since_epoch()).count();
    long endEpochSecs = std::chrono::duration_cast<std::chrono::seconds>(
            end.time_since_epoch()).count();

    ASSERT_GE(timestamp.EpochSecs(), startEpochSecs);
    ASSERT_LE(timestamp.EpochSecs(), endEpochSecs);
}


TEST(Timestamps,ReadFromTimestamp) {
    Time timestamp(reftime);
    AssertTimeMatches(timestamp);
    Time timestamp_with_cache(reftime);
    AssertTimeMatches(timestamp_with_cache);
}

TEST(Timestamps,ReadFromISOTimestamp) {
    Time timestamp(reftime_iso8601);
    AssertTimeMatches(timestamp);
}

TEST(Timestamps,InvalidTimestamps) {
    AssertIsEpoch("");
    AssertIsEpoch(nullptr);
    AssertIsEpoch("2017");
}

TEST(Timestamps,Copy) {
    Time timestamp(reftime);
    Time copy(timestamp);
    AssertTimeMatches(copy);
}

TEST(DiffSeconds,NoDiff) {
    ASSERT_EQ(Time(reftime).DiffSecs(reftime), 0);
}

TEST(DiffSeconds,YearDiff) {
    // TODO: Leap second handling...
    // TODO: Sub-second diff (microseconds etc)
    // TODO: 2016 was a leap year, and there was an additional leapSecond inserted on the 31st of Dec of 2016
    long threeYears = ((3 * 365 + 1) * 24 * 60 * 60);
    long diff = Time("20170403 10:11:02.194930").DiffSecs("20140403 10:11:02.194930");
    ASSERT_EQ(diff, threeYears);
}

TEST(DiffUSeconds,USecondDiff) {
    Time start ("20140403 10:11:02.394930");
    Time end ("20150504 11:11:03.294934");

    long diffusecs = (1430737863L - 1396519862L)*1000000L + (294934L - 394930L);
    ASSERT_EQ(end.DiffUSecs(start), diffusecs);
}
