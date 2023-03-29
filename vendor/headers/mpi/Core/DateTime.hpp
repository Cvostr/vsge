#pragma once 

#include "Base.hpp"
#include <string>

namespace Mpi {
    class DateTime {
    private:
        int64_t mTicks;
    public:
        DateTime(int64_t ticks);

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

        bool operator==(const DateTime& rhs) const;

        bool operator!=(const DateTime& rhs) const;

        DateTime operator+(const DateTime& rhs) const;

        DateTime operator-(const DateTime& rhs) const;

        void operator+=(const DateTime& rhs);

        void operator-=(const DateTime& rhs);

    private:
        static double getJulianDayFromTicks(int64_t ticks);

        static void calculateDate(int64_t ticks, int32_t& year, int32_t& month, int32_t& day);
    };
}