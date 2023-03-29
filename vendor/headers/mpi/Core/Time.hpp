#pragma once

#include "Base.hpp"

namespace Mpi {

    struct SystemTime {
        int year;
        int month;
        int dayOfWeek;
        int day;
        int hour;
        int minute;
        int second;
        int millisecond;

        SystemTime() : year(0), month(0), dayOfWeek(0), day(0), hour(0), minute(0), second(0), millisecond(0) {}

    };

    class Time {
    public:
        static uint64_t getCurrentTimeMillis();

        static SystemTime getLocalSystemTime();
        
        static SystemTime getUtcSystemTime();
    };
}