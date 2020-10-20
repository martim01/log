#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <map>

#ifdef __GNUWIN32__
    #ifdef LOG_DLL
        #define LOG_EXPORT __declspec(dllexport)
    #else
        #define LOG_EXPORT __declspec(dllimport)
    #endif // NMOS_EXPORT
#else
#define LOG_EXPORT
#endif

namespace pml
{
    class LogOutput;

    class LOG_EXPORT Log
    {
    public:
        typedef std::ostream&  (*ManipFn)(std::ostream&);
        typedef std::ios_base& (*FlagsFn)(std::ios_base&);

        static const std::string STR_LEVEL[6];

        enum enumLevel{ LOG_TRACE = 0, LOG_DEBUG=1, LOG_INFO=2, LOG_WARN=3, LOG_ERROR=4, LOG_CRITICAL=5 };

        static Log& Get(enumLevel eLevel=LOG_INFO);

        size_t AddOutput(std::unique_ptr<LogOutput> pLogout);

        void SetOutputLevel(size_t nIndex, enumLevel eLevel);
        void SetOutputLevel(enumLevel eLevel);
        void RemoveOutput(size_t nIndex);


        template<class T>  // int, double, strings, etc
        Log& operator<<(const T& output)
        {
            m_stream << output;
            return *this;
        }

        Log& operator<<(ManipFn manip);

        Log& operator<<(FlagsFn manip);

        Log& operator()(enumLevel e);
        void SetLevel(enumLevel e);

        void flush();

    private:
        Log();
        ~Log()
        {}

        std::stringstream  m_stream;
        enumLevel          m_logLevel;
        std::map<size_t, std::unique_ptr<LogOutput>> m_mOutput;
        size_t m_nOutputIdGenerator;

    };

    class LOG_EXPORT LogOutput
    {
        public:
            static const int TS_NONE            = 0;
            static const int TS_DATE            = 1;
            static const int TS_TIME            = 2;
            enum enumTS {TSR_SECOND, TSR_MILLISECOND, TSR_MICROSECOND, TSR_NANOSECOND};


            LogOutput(int nTimestamp=TS_TIME, enumTS eResolution=TSR_MILLISECOND) : m_eLevel(Log::LOG_TRACE), m_nTimestamp(nTimestamp), m_eResolution(eResolution){}
            virtual ~LogOutput(){}

            virtual void Flush(Log::enumLevel eLogLevel, const std::stringstream&  logStream);
            void SetOutputLevel(Log::enumLevel eLevel);
            Log::enumLevel GetOutputLevel() const;



        protected:
            Log::enumLevel m_eLevel;
            int m_nTimestamp;
            enumTS m_eResolution;
    };

};
