#pragma once

#include <string>
#include <exception>

namespace Mpi {

    enum ErrCode {
        ERRC_UNKNOWN_ERROR,
        ERRC_OBJECT_DESTROYED,
        ERRC_OTHER_ERROR
    };

    class Exception : public std::exception {
    private:
        int m_errCode;
    public:
        Exception(int errCode);

        Exception(int errCode, const char* msg);

        Exception(const char* msg);

        Exception(const std::string& msg);

        virtual ~Exception() {}

        const int getErrCode() const;
    };
}