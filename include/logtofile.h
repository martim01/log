#ifndef PML_LOG_FILE_H
#define PML_LOG_FILE_H

#include <fstream>
#include <string>

#include "dlllog.h"
#include "log.h"

#if ((defined(_MSVC_LANG) && _MSVC_LANG >=201703L) || __cplusplus >= 201703L)
#include <filesystem>
namespace pml::log
{
    /** @brief Output class that writes the log to a file. A log file is created for each hour and named YYYY-MM-DDTHH.log
    **/
    class LOG_EXPORT File : public Output
    {
        public:
            /** @brief Constructor
            *   @param sRootPath - the root path that the log files should live in.
            *   @param nTimestamp - the format of the timestamp that gets written in to the log
            *   @param eResolution - the resolution of the timestamp
            *   @param bLocalTime - whether to use local time or UTC time for the timestamp
            **/
           File(const std::filesystem::path& rootPath, int nTimestamp=kTsTime, TS resolution=TS::kMillisecond, bool bLocalTime=true);
            virtual ~File(){}


        private:

            void DoOutputMessage(Level level, const std::string&  logStream, const std::string& sPrefix) override;
            void Flush() override;

            void OpenFile(const std::string& sFileName);

            std::filesystem::path m_rootPath;
            std::string m_sCurrentFile;
            bool m_bLocalTime = true;
            std::ofstream m_ofLog;
            bool m_bOk = true;
    };
}
#else
namespace pml::log
{
    /** @brief Output class that writes the log to a file. A log file is created for each hour and named YYYY-MM-DDTHH.log
    **/
    class LOG_EXPORT File : public LogOutput
    {
        public:
            /** @brief Constructor
            *   @param sRootPath - the root path that the log files should live in.
            *   @param nTimestamp - the format of the timestamp that gets written in to the log
            *   @param eResolution - the resolution of the timestamp
            *   @param bLocalTime - whether to use local time or UTC time for the timestamp
            **/
           File(const std::string& sRootPath, int nTimestamp=ksTime, TS resolution=TS::kMillisecond, bool bLocalTime=true);
            virtual ~File(){}


        private:
            void DoOutputMessage(Level level, const std::string&  logStream, const std::string& sPrefix) override;
            void Flush() override;

            void OpenFile(const std::string& sFilePath, const std::string& sFileName);
            std::string m_sRootPath;
            std::string m_sFilePath;
            std::string m_sFileName;
            bool m_bLocalTime = true;
            std::ofstream m_ofLog;
            bool m_bOk = true;
    };
}
#endif

#endif