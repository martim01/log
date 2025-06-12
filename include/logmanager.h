#ifndef PML_LOG_MANAGER_H
#define PML_LOG_MANAGER_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <queue>
#include <thread>

#include "dlllog.h"
#include "log.h"

namespace pml::log
{
    
    class Manager
    {
        private:
            friend class Stream;

            static Manager& Get();
            Manager();
            ~Manager();

            size_t AddOutput(std::unique_ptr<Output> pLogout);
            void SetOutputLevel(size_t nIndex, Level level);
            void SetOutputLevel(Level level);
            void RemoveOutput(size_t nIndex);

            void Flush(const std::stringstream& ssLog, Level level, const std::string& sPrefix);

            void Loop();

            void Stop();

            void HandleQueue();

            std::map<size_t, std::unique_ptr<Output>> m_mOutput;
            size_t m_nOutputIdGenerator;


            struct logEntry
            {
                logEntry()=default;
                ~logEntry()=default;
                logEntry(const std::string& ss, Level e, const std::string& s) :
                    sLog(ss), level(e), sPrefix(s){}

                std::string sLog;
                Level level;
                std::string sPrefix;
            };

            std::queue<logEntry> m_qLog;

            std::mutex m_mutex;
            std::unique_ptr<std::thread> m_pThread = nullptr;
            std::atomic_bool m_bRun{true};

    };
}

#endif

