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
        int                m_logLevel;
        std::map<size_t, std::unique_ptr<LogOutput>> m_mOutput;
        size_t m_nOutputIdGenerator;

    };

    class LOG_EXPORT LogOutput
    {
        public:
            LogOutput() : m_eLevel(Log::LOG_TRACE){}
            virtual ~LogOutput(){}
            virtual void Flush(int nLogLevel, const std::stringstream&  logStream);
            void SetOutputLevel(Log::enumLevel eLevel);
            Log::enumLevel GetOutputLevel() const;
        protected:
            Log::enumLevel m_eLevel;
    };

};
