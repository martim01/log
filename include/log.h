#ifndef PML_LOG_LOG_H
#define PML_LOG_LOG_H


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

    namespace log
    {
        LOG_EXPORT const char* GetVersion();
        LOG_EXPORT const char* GetGitDate();
        LOG_EXPORT const char* GetGitTag();
        LOG_EXPORT const char* GetGitBranch();

        class Stream;

        /**
         * @brief the log level
         * 
         */
        enum class Level{ kTrace, kDebug, kInfo, kWarning, kError, kCritical };

        /** @brief helper function to easily access a LogStream. Usage is pml::log::log() << "this is a message";
        *   @param level the message level
        *   @return <i>LogStream</i>
        **/
        LOG_EXPORT Stream log(Level level = Level::kInfo, const std::string& sPrefix = "");
    
        /** @brief The Output class - the default class writes the log to the console, derive your own class from this to write the log elsewhere
        **/
        class LOG_EXPORT Output
        {
            public:

                static constexpr int kTsNone            = 0;    ///< don't prepend messages
                static constexpr int kTsDate            = 1;    ///< prepend messages with date
                static constexpr int kTsTime            = 2;    ///< prepend messages with time
                /** @enum the timestamp resolution
                **/
                enum class TS {kSecond, kMillisecond, kMicrosecond, kNanosecond};

                /** @brief Constructor
                *   @param nTimestamp the format of the time message to write for each log message. Can be TS_NONE (no message), TS_DATE (yyyy-mm-dd) and/or TS_TIME (hh:mm:ss)
                *   @param eResolution the resolution of the timestamp message if TS_TIME is set
                **/
                Output(int nTimestamp=kTsTime, TS resolution=TS::kMillisecond) : m_level(Level::kTrace), m_nTimestamp(nTimestamp), m_resolution(resolution){}
                
                /**
                 * @brief Destroy the Output object
                 * 
                 */
                virtual ~Output(){}

                /** @brief Called by the LogStream when it needs to be flushed - should not be called directly
                *   @param eLogLevel the level of the current message that is being flushed
                *   @param sLog the current message
                **/
                virtual void Flush(Level level, const std::string&  sLog, const std::string& sPrefix);

                /** @brief Sets the level that a log message must meet to be output by the LogOutput
                *   @param eLevel the level
                **/
                void SetOutputLevel(Level level);

                /** @brief Gets the output level that a log message must meet to be output by the LogOutput
                *   @return  the level
                **/
                Level GetOutputLevel() const;


            protected:
                std::stringstream Timestamp();
                Level m_level;
                int m_nTimestamp;
                TS m_resolution;
        };


        /** @brief the main logging class
        **/
        class LOG_EXPORT Stream
        {
        public:

            /** @brief Constructor
            *   @param eLevel the level of the current message
            *   @deprecated use the log::Level version instead
            **/
           Stream(pml::enumLevel eLevel, const std::string& sPrefix);


            /** @brief Constructor
            *   @param level the level of the current message
            **/
           Stream(Level level = Level::kInfo, const std::string& sPrefix="");

            ///< @brief Copy Constructor
            Stream(const Stream& lg);

            ///< @brief Assignment operator
            Stream& operator=(const Stream& lg);

            ~Stream();

            typedef std::ostream&  (*ManipFn)(std::ostream&);
            typedef std::ios_base& (*FlagsFn)(std::ios_base&);


            static const std::string STR_LEVEL[6];

            /** @brief Add a Output derived object to all subsequent log messages
            *   @param pLogout the Output device
            *   @return <i>size_t</i> the index of the added output
            **/
            static size_t AddOutput(std::unique_ptr<Output> pLogout);

            /** @brief Sets the level a message must meet in order to be output by the Output with the given index
            *   @param nIndex the index of the Output
            *   @param eLevel the level
            *   @deprecated
            **/
            static void SetOutputLevel(size_t nIndex, pml::enumLevel eLevel);

            /** @brief Sets the level a message must meet in order to be output by the Output with the given index
            *   @param nIndex the index of the Output
            *   @param level the level
            **/
            static void SetOutputLevel(size_t nIndex, Level level);

            /** @brief Sets the level a message must meet in order to be output by all the LogOutputs
            *   @param eLevel the level
            *   @deprecated
            **/
            static void SetOutputLevel(pml::enumLevel eLevel);

            /** @brief Sets the level a message must meet in order to be output by all the LogOutputs
            *   @param level the level
            **/
            static void SetOutputLevel(Level level);

            /** @brief Removes the LogOutput with the given index
            *   @param nIndex the index of the LogOutput
            **/
            static void RemoveOutput(size_t nIndex);

            /** @brief Stops the logging thread
            **/
            static void Stop();


            /**
             * @brief logging stream operator for ints, doubles, strings etc
             * 
             * @tparam T 
             * @param output 
             * @return Stream& 
             */
            template<class T>  // int, double, strings, etc
            Stream& operator<<(const T& output)
            {
                m_stream << output;
                return *this;
            }

            
            Stream& operator<<(ManipFn manip);

            Stream& operator<<(FlagsFn manip);

            Stream& operator()(Level level=Level::kInfo);
            Stream& SetLevel(Level level);

            void flush();

        protected:
            const std::stringstream& GetStream() const
            {
                return m_stream;
            }
            Level GetLevel() const
            {
                return m_level;
            }
            const std::string& GetPrefix() const { return m_sPrefix; }

        private:


            std::stringstream m_stream;
            Level m_level;
            std::string m_sPrefix;

        };
    }

  
}

/** @brief helper function to easily access a LogStream. Usage is pmlLog() << "this is a message";
*   @param eLevel the message level
*   @return <i>LogStream</i>
*   @deprecated use pml::log::log() instead
**/
LOG_EXPORT pml::log::Stream pmlLog(pml::enumLevel elevel = pml::LOG_INFO, const std::string& sPrefix="");


#endif