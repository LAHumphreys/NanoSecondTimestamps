#include "util_time.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>


using namespace std;

const char* Time::EpochTimestamp =  "19700101 00:00:00.000000";

namespace {
    const time_t& get_epoch() {
        thread_local time_t epoch;
        thread_local bool initialised = false;

        if (!initialised) {
            struct tm epoch_tm;
            epoch_tm.tm_sec = 0;
            epoch_tm.tm_min = 0;
            epoch_tm.tm_hour= 0;
            epoch_tm.tm_mday= 1;
            epoch_tm.tm_mon=0;
            epoch_tm.tm_year=70;
            epoch_tm.tm_isdst=0;
            epoch_tm.tm_gmtoff = 0;

            epoch = timegm(&epoch_tm);

            initialised = true;
        }

        return epoch;
    }
}

Time::Time() {
    SetNow();
}

Time::Time(const Time& rhs) {
    (*this) = rhs;
}

Time::Time(const std::string& timestamp) {
    (*this) = timestamp;
}

Time::Time(const char* timestamp) {
    (*this) = timestamp;
}

Time::Time(const struct timeval& tv) {
    (*this) = tv;
}

Time::Time(const long& usecs) {
    (*this) = usecs;
}

Time& Time::operator=(const struct timeval& tv) {
    this->tv = tv;
    ready = false;
    return *this;
}

Time& Time::operator=(const long& usecs ) {
    this->tv.tv_usec = usecs%1000000L;
    this->tv.tv_sec = usecs / 1000000L;
    ready = false;
    return *this;
}

Time& Time::operator=(const Time& rhs) {
    this->tv = rhs.tv;
    this->ready = false;
    return *this;
}

Time& Time::operator=(const std::string& timestamp) {
    InitialiseFromString(timestamp.c_str(),timestamp.length());
    return *this;
}

Time& Time::operator=(const char* timestamp) {
    if (timestamp) {
        InitialiseFromString(timestamp,strlen(timestamp));
    } else {
        InitialiseBlank();
    };
    return *this;
}

void Time::InitialiseFromString(const char* str, size_t len) {

    if (len >= 24)
    {
        char buf[28];
        strncpy(buf,str,27);
        buf[27] = 0;

        if ( len >= 27 && buf[4] == '-' )
        {
            InitialiseFromISOTimestamp(buf);
        }
        else
        {
            InitialiseFromTimestamp(buf);
        }
    }
    else
    {
        InitialiseBlank();
    }

    ready = true;
}

void Time::InitialiseFromTimestamp(char* buf)
{
    /*
     * Initialise the time to Midnight on the 1st of January 1970
     * ----------------------------------------------------------
     */
    struct tm epoch;
    epoch.tm_sec = 0;
    epoch.tm_min = 0;
    epoch.tm_hour= 0;
    epoch.tm_mday= 1;
    epoch.tm_mon=0;
    epoch.tm_year=70;
    epoch.tm_isdst=0;
    epoch.tm_gmtoff = 0;
    //epoch.tm_wday: Not used by timegm
    //epoch.tm_yday: Not used by timegm
    time = epoch;

    /*
     * Pull apart the string
     * ---------------------
     *  Exected format: YYYYMMDD HH:MM:SS.UUUUUU
     *  Index:          012345678901234567890123
     */

    /*
     * atoi (whilst nasty) is ok here since we are sure of null termination.
     *
     * In the case where we have garbage, atoi will return 0, which is
     * good enough to represent garbage data.
     */
    tv.tv_usec =  atoi(buf + 18);
    buf[17] = 0;
    time.tm_sec  = atoi(buf+15);

    buf[14] = 0;
    time.tm_min  = atoi(buf+12);

    buf[11] = 0;
    time.tm_hour = atoi(buf+9);

    buf[8] = 0;
    time.tm_mday = atoi(buf+6);

    buf[6] = 0;
    time.tm_mon = atoi(buf+4);

    buf[4] = 0;
    time.tm_year = atoi(buf);

    if ( time.tm_year != 0 ) {
        time.tm_year -=1900;
    }

    if ( time.tm_mon != 0 ) {
        --time.tm_mon;
    }

     /*
     * calculate timeval...
     * --------------------
     */   
     tv.tv_sec = difftime(timegm(&time),get_epoch());
}

void Time::InitialiseFromISOTimestamp(char* buf)
{
    /*
     * Initialise the time to Midnight on the 1st of January 1970
     * ----------------------------------------------------------
     */
    struct tm epoch;
    epoch.tm_sec = 0;
    epoch.tm_min = 0;
    epoch.tm_hour= 0;
    epoch.tm_mday= 1;
    epoch.tm_mon=0;
    epoch.tm_year=70;
    epoch.tm_isdst=0;
    epoch.tm_gmtoff = 0;
    //epoch.tm_wday: Not used by timegm
    //epoch.tm_yday: Not used by timegm
    time = epoch;

    /*
     * Pull apart the string
     * ---------------------
     *  Exected format: YYYY-MM-DDTHH:MM:SS.UUUUUUZ
     *  Index:          012345678901234567890123456
     */

    /*
     * atoi (whilst nasty) is ok here since we are sure of null termination.
     *
     * In the case where we have garbage, atoi will return 0, which is
     * good enough to represent garbage data.
     */
    buf[26] = 0;
    tv.tv_usec =  atoi(buf + 20);

    buf[19] = 0;
    time.tm_sec  = atoi(buf+17);

    buf[16] = 0;
    time.tm_min  = atoi(buf+14);

    buf[13] = 0;
    time.tm_hour = atoi(buf+11);

    buf[10] = 0;
    time.tm_mday = atoi(buf+8);

    buf[7] = 0;
    time.tm_mon = atoi(buf+5);

    buf[4] = 0;
    time.tm_year = atoi(buf);

    if ( time.tm_year != 0 ) {
        time.tm_year -=1900;
    }

    if ( time.tm_mon != 0 ) {
        --time.tm_mon;
    }

     /*
     * calculate timeval...
     * --------------------
     */   
     tv.tv_sec = difftime(timegm(&time),get_epoch());
}

void Time::InitialiseBlank()
{
    struct tm epoch;
    epoch.tm_sec = 0;
    epoch.tm_min = 0;
    epoch.tm_hour= 0;
    epoch.tm_mday= 1;
    epoch.tm_mon=0;
    epoch.tm_year=70;
    epoch.tm_isdst=0;
    epoch.tm_gmtoff = 0;
    //epoch.tm_wday: Not used by timegm
    //epoch.tm_yday: Not used by timegm
    time = epoch;

     tv.tv_sec = difftime(get_epoch(),get_epoch());
     tv.tv_usec =  0;
}

Time& Time::SetNow() {
    gettimeofday(&tv,NULL);
    ready = false;
    return *this;
}

void Time::MakeReady() const {
    if ( !ready) {
        SetTmFromTimeval();
    }
}

void Time::SetTmFromTimeval() const {
    gmtime_r(&tv.tv_sec,&time);
}

string Time::ISO8601Timestamp() const {
    MakeReady();
    stringstream strtime;
    strtime << Year();
    strtime << setw(1) << "-";
    strtime << setw(2) << setfill ('0') << Month();
    strtime << setw(1) << "-";
    strtime << setw(2) << setfill('0') << MDay();
    strtime << setw(1) << "T";
    strtime << setw(2) << setfill('0') << Hour();
    strtime << setw(1) << ":";
    strtime << setw(2) << Minute(); 
    strtime << setw(1) << ":";
    strtime << setw(2) << setfill('0') << Second();
    strtime << setw(1) << ".";
    strtime << setw(6) <<  setfill('0') << USec();
    strtime << "Z";
    return strtime.str();
}

string Time::Timestamp() const {
    MakeReady();
    stringstream strtime;
    strtime << Year();
    strtime << setw(2) << setfill ('0');
    strtime << Month();
    strtime << setw(2) << setfill('0') << MDay();
    strtime << setw(1) << " ";
    strtime << setw(2) << setfill('0') << Hour();
    strtime << setw(1) << ":";
    strtime << setw(2) << Minute(); 
    strtime << setw(1) << ":";
    strtime << setw(2) << setfill('0') << Second();
    strtime << setw(1) << ".";
    strtime << setw(6) <<  setfill('0') << USec();
    return strtime.str();
}

string Time::FileTimestamp() const {
    MakeReady();
    stringstream strtime;
    strtime << Year() << "-" << Month() << "-" << MDay();
    strtime << "_" << Hour() << ":" << Minute() << ":" << Second();
    strtime << "-" << USec();
    return strtime.str();
}

int Time::DiffSecs(const Time& rhs) const {
    int diff = (tv.tv_sec - rhs.tv.tv_sec);
    if ( tv.tv_usec < rhs.tv.tv_usec) {
        diff-=1;
    }
    return diff;
}
long Time::DiffUSecs(const Time& rhs) const {
    long diff = (  static_cast<long>(tv.tv_sec) - 
                   static_cast<long>(rhs.tv.tv_sec)
                )*1000000;
    diff+=(tv.tv_usec - rhs.tv.tv_usec);
    return diff;
}

long Time::EpochUSecs() const {
    return tv.tv_usec + 1e6L* (long(tv.tv_sec));
}

int Time::EpochSecs() const {
    return tv.tv_sec;
}
