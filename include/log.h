#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <map>

#ifdef __GNUWIN32__
    #ifdef BUILD_DLL
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

        size_t AddOutput(std::unique_ptr<LogOutput> pLogout)
        {
            ++m_nOutputIdGenerator;
            return m_mOutput.insert(std::make_pair(m_nOutputIdGenerator, move(pLogout))).first->first;
        }

        void RemoveOutput(size_t nIndex)
        {
            m_mOutput.erase(nIndex);
        }

        void SetOutputLevel(size_t nIndex, enumLevel eLevel);
        void SetOutputLevel(enumLevel eLevel);


        template<class T>  // int, double, strings, etc
        Log& operator<<(const T& output)
        {
            m_stream << output;
            return *this;
        }

        Log& operator<<(ManipFn manip) /// endl, flush, setw, setfill, etc.
        {
            manip(m_stream);

            if (manip == static_cast<ManipFn>(std::flush)
             || manip == static_cast<ManipFn>(std::endl ) )
                this->flush();

            return *this;
        }

        Log& operator<<(FlagsFn manip) /// setiosflags, resetiosflags
        {
            manip(m_stream);
            return *this;
        }

        Log& operator()(enumLevel e)
        {
            m_logLevel = e;
            return *this;
        }

        void SetLevel(enumLevel e)
        {
            m_logLevel = e;
        }

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
