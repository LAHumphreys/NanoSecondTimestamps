#include "util_time.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>


using namespace std;
using namespace nstimestamp;

const char* Time::EpochTimestamp =  "19700101 00:00:00.000000000";

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

Time& Time::operator=(const struct timeval& tv) {
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
    data.ready = false;
    return *this;
}

Time& Time::operator=(const Time& rhs) {
    this->ts = rhs.ts;
    data.ready = false;
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

        if ( len >= 5 && buf[4] == '-' )
        {
            InitialiseFromISOTimestamp(buf);
        }
        else
        {
            InitialiseFromTimestamp(buf, len);
        }
    }
    else
    {
        InitialiseBlank();
    }

    data.ready = true;
}

void Time::InitialiseFromTimestamp(char* buf, const size_t len)
{
    /*
     * Initialise the TM() to Midnight on the 1st of January 1970
     * ----------------------------------------------------------
     */
    struct tm epoch, working;
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
    working = epoch;

    /*
     * Pull apart the string
     * ---------------------
     *  Exected format: YYYYMMDD HH:MM:SS.MMMUUUNNN
     *  Index:          012345678901234567890123456
     */

    /*
     * atoi (whilst nasty) is ok here since we are sure of null termination.
     *
     * In the case where we have garbage, atoi will return 0, which is
     * good enough to represent garbage data.
     */

    if (len > 24) {
        ts.tv_nsec =  atoi(buf + 18);
    } else {
        // careful - old timestamps will have been created without the nanos...
        ts.tv_nsec =  atoi(buf + 18) * 1000;
    }
    buf[17] = 0;
    working.tm_sec  = atoi(buf+15);

    buf[14] = 0;
    working.tm_min  = atoi(buf+12);

    buf[11] = 0;
    working.tm_hour = atoi(buf+9);

    buf[8] = 0;
    working.tm_mday = atoi(buf+6);

    buf[6] = 0;
    working.tm_mon = atoi(buf+4);

    buf[4] = 0;
    working.tm_year = atoi(buf);

    if ( working.tm_year != 0 ) {
        working.tm_year -=1900;
    }

    if ( working.tm_mon != 0 ) {
        --working.tm_mon;
    }

     /*
     * calculate timeval...
     * --------------------
     */   
    ts.tv_sec = difftime(timegm(&working),get_epoch());
    SetTM(working);
}

void Time::InitialiseFromISOTimestamp(char* buf)
{
    /*
     * Initialise the TM() to Midnight on the 1st of January 1970
     * ----------------------------------------------------------
     */
    struct tm epoch, working;
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
    working = epoch;

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
    ts.tv_nsec =  atoi(buf + 20) * 1000;

    buf[19] = 0;
    working.tm_sec  = atoi(buf+17);

    buf[16] = 0;
    working.tm_min  = atoi(buf+14);

    buf[13] = 0;
    working.tm_hour = atoi(buf+11);

    buf[10] = 0;
    working.tm_mday = atoi(buf+8);

    buf[7] = 0;
    working.tm_mon = atoi(buf+5);

    buf[4] = 0;
    working.tm_year = atoi(buf);

    if ( working.tm_year != 0 ) {
        working.tm_year -=1900;
    }

    if ( working.tm_mon != 0 ) {
        --working.tm_mon;
    }

    /*
     * calculate timeval...
     * --------------------
     */
    ts.tv_sec = difftime(timegm(&working),get_epoch());
    SetTM(working);
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
    SetTM(epoch);

    ts.tv_sec = difftime(get_epoch(),get_epoch());
    ts.tv_nsec =  0;
}

Time& Time::SetNow() {
    clock_gettime(CLOCK_REALTIME, &ts);
    data.ready = false;
    return *this;
}

void Time::MakeReady() const {
    if ( !data.ready) {
        data.ready = true;
        SetTmFromTimeval();
    }
}

void Time::SetTmFromTimeval() const {
    tm buf;
    gmtime_r(&ts.tv_sec,&buf);
    SetTM(buf);
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
    strtime << setw(9) <<  setfill('0') << NSec();
    return strtime.str();
}

int Time::DiffSecs(const Time& rhs) const {
    int diff = (ts.tv_sec - rhs.ts.tv_sec);
    if ( ts.tv_nsec < rhs.ts.tv_nsec) {
        diff-=1;
    }
    return diff;
}
long Time::DiffUSecs(const Time& rhs) const {
    long diff = (  static_cast<long>(ts.tv_sec) -
                   static_cast<long>(rhs.ts.tv_sec)
                )*1000000;
    diff+=((ts.tv_nsec - rhs.ts.tv_nsec) / 1000);
    return diff;
}

long Time::EpochUSecs() const {
    return ts.tv_nsec / 1000 + 1e6L* (long(ts.tv_sec));
}

long Time::EpochNSecs() const {
    return ts.tv_nsec + 1e9L* (long(ts.tv_sec));
}


int Time::EpochSecs() const {
    return ts.tv_sec;
}

void Time::SetTM(const tm &time) const {
    data.tm_sec = time.tm_sec;
    data.tm_min = time.tm_min;
    data.tm_hour = time.tm_hour;
    data.tm_mday = time.tm_mday;
    data.tm_mon = time.tm_mon;
    data.tm_year = time.tm_year;
}

