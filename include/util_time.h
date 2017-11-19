/**
 * (c) Luke Humphreys 2017
 */
#ifndef __ELF_64_UTIL_TIME__
#define __ELF_64_UTIL_TIME__

#include <sys/time.h>
#include <string>

namespace nstimestamp {

class Time {
public:
    // Initialise with the current time
    Time ();
    // Initialise as a copy of another time
    Time (const Time& othertime);
    /**
     * Initialise by parsing the provided timestamp, which be either:
     *   1. A string produced by Timestamp()
     *   2. An ISO8601 Timestamp
     */
    Time (const std::string& timestamp);

    /**
     * Less efficient string c'tor (requires an additional strlen)
     */
    Time (const char* timestamp);

    /**
     * Initialise from a system timeval object
     */
    Time (const struct timeval& tv);


    // Assignment operators, behave as c'tors...
    Time& operator=(const Time& rhs);
    Time& operator=(const std::string& timestamp);
    Time& operator=(const char* timestamp);
    Time& operator=(const struct timeval& tv);

    // Reset the time object to the current time
    Time& SetNow();

    // Reset the time object to the value of str.
    void InitialiseFromString(const char* str, size_t len);

    // Components
    int Year()   const { MakeReady(); return data.tm_year + 1900;}
    int Month()  const { MakeReady(); return data.tm_mon + 1;}
    int MDay()   const { MakeReady(); return data.tm_mday;}
    int Hour()   const { MakeReady(); return data.tm_hour;}
    int Minute() const { MakeReady(); return data.tm_min;}
    int Second() const { MakeReady(); return data.tm_sec;}
    int MSec()   const { return ts.tv_nsec / 1000000;}
    int USec()   const { return ts.tv_nsec / 1000;}
    int NSec()   const { return ts.tv_nsec;}

    // Diffs: Time since rhs: (this - rhs)
    int  DiffSecs (const Time& rhs) const;
    long DiffUSecs (const Time& rhs) const;

    // micro-seconds since the epoch
    int EpochSecs() const;
    long EpochUSecs() const;
    long EpochNSecs() const;

    /**
     * Produce a human readible Timestamp of the form:
     *     YYYYMMDD HH:MM:SS.MMMUUUNNN
     */
    std::string Timestamp() const;

    /**
     * Produce a human readible Timestamp of the form:
     *     YYYY-MM-DDTHH:MM:SS.UUUUUUZ
     */
    std::string ISO8601Timestamp() const;


    // Initialise to the value of Timestamp() at the Epoch
    static const char* EpochTimestamp;
private:
    void MakeReady() const;
    void SetTmFromTimeval() const;


    /**
     * Initialise from a timestamp in the format provided by Timestamp();
     *
     * @param buf  A copy of the timestamp, will be edited by the function.
     */
    void InitialiseFromTimestamp(char* buf, const size_t len);

    /**
     * Initialise from a timestamp in the format provided by ISO8601Timestamp();
     *
     * @param buf  A copy of the timestamp, will be edited by the function.
     */
    void InitialiseFromISOTimestamp(char* buf);

    void InitialiseBlank();

    void SetTM(const tm& time) const;

    /**
     * Slimmed down version of the tm struct.
     *
     * If we use a native tm struct then we'll no longer fit into a single
     * cache-line on X86_64 systems.
     */
    struct TMLight {
        int8_t ready;			// ready flag
        int8_t tm_sec;			/* Seconds.	[0-60] (1 leap second) */
        int8_t tm_min;			/* Minutes.	[0-59] */
        int8_t tm_hour;			/* Hours.	[0-23] */
        int8_t tm_mday;			/* Day.		[1-31] */
        int8_t tm_mon;			/* Month.	[0-11] */
        int16_t tm_year;		/* Year	- 1900.  */
    };

    timespec ts;
    mutable TMLight data;
};
}

#endif
