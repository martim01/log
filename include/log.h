#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <map>
#include <thread>
#include <mutex>

#include "dlllog.h"

namespace pml
{
    enum enumLevel{ LOG_TRACE = 0, LOG_DEBUG=1, LOG_INFO=2, LOG_WARN=3, LOG_ERROR=4, LOG_CRITICAL=5 };

    class LOG_EXPORT LogOutput
    {
        public:
            static const int TS_NONE            = 0;
            static const int TS_DATE            = 1;
            static const int TS_TIME            = 2;
            enum enumTS {TSR_SECOND, TSR_MILLISECOND, TSR_MICROSECOND, TSR_NANOSECOND};


            LogOutput(int nTimestamp=TS_TIME, enumTS eResolution=TSR_MILLISECOND) : m_eLevel(LOG_TRACE), m_nTimestamp(nTimestamp), m_eResolution(eResolution){}
            virtual ~LogOutput(){}

            virtual void Flush(enumLevel eLogLevel, const std::stringstream&  logStream);
            void SetOutputLevel(enumLevel eLevel);
            enumLevel GetOutputLevel() const;



        protected:
            std::stringstream Timestamp();
            enumLevel m_eLevel;
            int m_nTimestamp;
            enumTS m_eResolution;
    };

    class LOG_EXPORT LogStream
    {
    public:

        LogStream(enumLevel eLevel = LOG_INFO);
        LogStream(const LogStream& lg);
        LogStream& operator=(const LogStream& lg);

        ~LogStream();

        typedef std::ostream&  (*ManipFn)(std::ostream&);
        typedef std::ios_base& (*FlagsFn)(std::ios_base&);

        static const std::string STR_LEVEL[6];

        static size_t AddOutput(std::unique_ptr<LogOutput> pLogout);
        static void SetOutputLevel(size_t nIndex, enumLevel eLevel);
        static void SetOutputLevel(enumLevel eLevel);
        static void RemoveOutput(size_t nIndex);


        template<class T>  // int, double, strings, etc
        LogStream& operator<<(const T& output)
        {
            m_stream << output;
            return *this;
        }

        LogStream& operator<<(ManipFn manip);

        LogStream& operator<<(FlagsFn manip);

        LogStream& operator()(enumLevel e=LOG_INFO);
        void SetLevel(enumLevel e);

        void flush();

    protected:
        const std::stringstream& GetStream() const
        {
            return m_stream;
        }
        enumLevel GetLevel() const
        {
            return m_logLevel;
        }

    private:


        std::stringstream m_stream;
        enumLevel          m_logLevel;

    };

   // LOG_EXPORT LogStream Log(enumLevel elevel = LOG_INFO);
};

LOG_EXPORT pml::LogStream pmlLog(pml::enumLevel elevel = pml::LOG_INFO);

