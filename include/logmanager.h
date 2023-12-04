#pragma once
#include "log.h"
#include "dlllog.h"

namespace pml
{
    class LogOutput;
    class LogManager
    {
        private:
            friend class LogStream;

            static LogManager& Get();
            LogManager();

            size_t AddOutput(std::unique_ptr<LogOutput> pLogout);
            void SetOutputLevel(size_t nIndex, enumLevel eLevel);
            void SetOutputLevel(enumLevel eLevel);
            void RemoveOutput(size_t nIndex);

            void Flush(const std::stringstream& ssLog, enumLevel eLevel, const std::string& sPrefix);

            std::mutex m_mutex;

            std::map<size_t, std::unique_ptr<LogOutput>> m_mOutput;
            size_t m_nOutputIdGenerator;

    };
}

