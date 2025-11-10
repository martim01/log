#ifndef HEADER_890DAD0FDEE7D13A
#define HEADER_890DAD0FDEE7D13A

#pragma once
#include "log.h"
#include "dlllog.h"
#include <queue>
#include <thread>
#include <memory>
#include <atomic>
#include <condition_variable>

namespace pml
{
    class LogOutput;
    class LogManager
    {
        private:
            friend class LogStream;

            static LogManager& Get();
            LogManager();
            ~LogManager();

            size_t AddOutput(std::unique_ptr<LogOutput> pLogout);
            void SetOutputLevel(size_t nIndex, enumLevel eLevel);
            void SetOutputLevel(enumLevel eLevel);
            void RemoveOutput(size_t nIndex);

            void Flush(const std::stringstream& ssLog, enumLevel eLevel, const std::string& sPrefix);

            void Loop();
            void HandleQueue();
            void Stop();

            std::map<size_t, std::unique_ptr<LogOutput>> m_mOutput;
            size_t m_nOutputIdGenerator;


            struct logEntry
            {
                logEntry()=default;
                ~logEntry()=default;
                logEntry(const std::string& ss, enumLevel e, const std::string& s) :
                    sLog(ss), eLevel(e), sPrefix(s){}

                std::string sLog;
                enumLevel eLevel;
                std::string sPrefix;
            };

            std::queue<logEntry> m_qLog;


            std::mutex m_mutex;
            std::unique_ptr<std::thread> m_pThread = nullptr;
            std::atomic_bool m_bRun{true};
            std::condition_variable m_cv;

    };
}

#endif // header guard

