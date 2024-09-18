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

    /** @class The LogOutput class - the default class writes the log to the console, derive your own class from this to write the log elsewhere
    **/
    class LOG_EXPORT LogOutput
    {
        public:

            static const int TS_NONE            = 0;
            static const int TS_DATE            = 1;
            static const int TS_TIME            = 2;
            /** @enum the timestamp resolution
            **/
            enum enumTS {TSR_SECOND, TSR_MILLISECOND, TSR_MICROSECOND, TSR_NANOSECOND};

            /** @brief Constructor
            *   @param nTimestamp the format of the time message to write for each log message. Can be TS_NONE (no message), TS_DATE (yyyy-mm-dd) and/or TS_TIME (hh:mm:ss)
            *   @param eResolution the resolution of the timestamp message if TS_TIME is set
            **/
            LogOutput(int nTimestamp=TS_TIME, enumTS eResolution=TSR_MILLISECOND) : m_eLevel(LOG_TRACE), m_nTimestamp(nTimestamp), m_eResolution(eResolution){}
            virtual ~LogOutput(){}

            /** @brief Called by the LogStream when it needs to be flushed - should not be called directly
            *   @param eLogLevel the level of the current message that is being flushed
            *   @param logStream the current message
            **/
            virtual void Flush(enumLevel eLogLevel, const std::stringstream&  logStream, const std::string& sPrefix);

            /** @brief Sets the level that a log message must meet to be output by the LogOutput
            *   @param eLevel the level
            **/
            void SetOutputLevel(enumLevel eLevel);

            /** @brief Gets the output level that a log message must meet to be output by the LogOutput
            *   @return <i>enumLevel</i> the level
            **/
            enumLevel GetOutputLevel() const;



        protected:
            std::stringstream Timestamp();
            enumLevel m_eLevel;
            int m_nTimestamp;
            enumTS m_eResolution;
    };

    /** @class the main logging class
    **/
    class LOG_EXPORT LogStream
    {
    public:

        /** @brief Constructor
        *   @param eLevel the level of the current message
        **/
        LogStream(enumLevel eLevel = LOG_INFO, const std::string& sPrefix="");

        ///< @brief Copy Constructor
        LogStream(const LogStream& lg);

        ///< @brief Assignment operator
        LogStream& operator=(const LogStream& lg);

        ~LogStream();

        typedef std::ostream&  (*ManipFn)(std::ostream&);
        typedef std::ios_base& (*FlagsFn)(std::ios_base&);


        static const std::string STR_LEVEL[6];

        /** @brief Add a LogOutput derived object to all subsequent log messages
        *   @param pLogout the LogOutput device
        *   @return <i>size_t</i> the index of the added output
        **/
        static size_t AddOutput(std::unique_ptr<LogOutput> pLogout);

        /** @brief Sets the level a message must meet in order to be output by the LogOutput with the given index
        *   @param nIndex the index of the LogOutput
        *   @param eLevel the level
        **/
        static void SetOutputLevel(size_t nIndex, enumLevel eLevel);

        /** @brief Sets the level a message must meet in order to be output by all the LogOutputs
        *   @param eLevel the level
        **/
        static void SetOutputLevel(enumLevel eLevel);

        /** @brief Removes the LogOutput with the given index
        *   @param nIndex the index of the LogOutput
        **/
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
        LogStream& SetLevel(enumLevel e);

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
        const std::string& GetPrefix() const { return m_sPrefix; }

    private:


        std::stringstream m_stream;
        enumLevel          m_logLevel;
        std::string m_sPrefix;

    };

   namespace log
   {
      LOG_EXPORT const char* GetVersion();
      LOG_EXPORT const char* GetGitDate();
      LOG_EXPORT const char* GetGitTag();
      LOG_EXPORT const char* GetGitBranch();
   }
};

/** @brief helper function to easily access a LogStream.
Usage is pmlLog() << "this is a message";
*   @param eLevel the message level
*   @return <i>LogStream</i>
**/
LOG_EXPORT pml::LogStream pmlLog(pml::enumLevel elevel = pml::LOG_INFO, const std::string& sPrefix="");


