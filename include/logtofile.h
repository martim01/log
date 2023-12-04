#pragma once
#include "log.h"
#include <string>
#include <fstream>
#include "dlllog.h"
#if ((defined(_MSVC_LANG) && MSVC_LANG >=201703L) || __cplusplus >= 201703L)
#include <filesystem>
namespace pml
{
    /** @class LogOutput class that writes the log to a file. A log file is created for each hour and named YYYY-MM-DDTHH.log
    **/
    class LOG_EXPORT LogToFile : public LogOutput
    {
        public:
            /** @brief Constructor
            *   @param sRootPath - the root path that the log files should live in.
            *   @param nTimestamp - the format of the timestamp that gets written in to the log
            *   @param eResolution - the resolution of the timestamp
            **/
            LogToFile(const std::filesystem::path& rootPath, int nTimestamp=TS_TIME, enumTS eResolution=TSR_MILLISECOND);
            virtual ~LogToFile(){}

            /** @brief Called by the LogStream when it needs to be flushed - should not be called directly
            *   @param eLogLevel the level of the current message that is being flushed
            *   @param logStream the current message
            **/
            void Flush(pml::enumLevel eLogLevel, const std::stringstream&  logStream, const std::string& sPrefix) override;

        private:

            void OpenFile(const std::string& sFileName);

            std::filesystem::path m_rootPath;
            std::string m_sCurrentFile;
            
            std::ofstream m_ofLog;
            bool m_bOk = true;
    };
}
#else
namespace pml
{
    /** @class LogOutput class that writes the log to a file. A log file is created for each hour and named YYYY-MM-DDTHH.log
    **/
    class LOG_EXPORT LogToFile : public LogOutput
    {
        public:
            /** @brief Constructor
            *   @param sRootPath - the root path that the log files should live in.
            *   @param nTimestamp - the format of the timestamp that gets written in to the log
            *   @param eResolution - the resolution of the timestamp
            **/
            LogToFile(const std::string& sRootPath, int nTimestamp=TS_TIME, enumTS eResolution=TSR_MILLISECOND);
            virtual ~LogToFile(){}

            /** @brief Called by the LogStream when it needs to be flushed - should not be called directly
            *   @param eLogLevel the level of the current message that is being flushed
            *   @param logStream the current message
            **/
            void Flush(pml::enumLevel eLogLevel, const std::stringstream&  logStream) override;

        private:

            void OpenFile(const std::string& sFilePath, const std::string& sFileName);
            std::string m_sRootPath;
            std::string m_sFilePath;
            std::string m_sFileName;

            std::ofstream m_ofLog;
            bool m_bOk = true;
    };
}
#endif