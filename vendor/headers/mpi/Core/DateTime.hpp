#pragma once 

#include "Base.hpp"
#include <string>

namespace Mpi {
    class DateTime {
    private:
        int64 mTicks;
    public:
        DateTime(int64 ticks);

        DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond = 0);

        int getYear() const;

        int getMonth() const;

        int getDay() const;

        int getHour() const;

        int getMinute() const;

        int getSecond() const;

        bool isAfter(const DateTime& dateTime) const;

        int64_t getTicks() const;

        static DateTime now();

        static DateTime nowUtc();

        static DateTime unixEpoch();

        bool isYearLeap(int year);

        std::string toString() const;

        DateTime operator==(const DateTime& rhs) const;

        DateTime operator!=(const DateTime& rhs) const;

        DateTime operator+(const DateTime& rhs) const;

        DateTime operator-(const DateTime& rhs) const;

        void operator+=(const DateTime& rhs);

        void operator-=(const DateTime& rhs);

    private:
        static double getJulianDayFromTicks(int64 ticks);

        static void calculateDate(int64 ticks, int32& year, int32& month, int32& day);
    };
}