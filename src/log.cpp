#include "log.h"
#include "logmanager.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include "log_version.h"



const char* GetVersion()    {   return pml::log::VERSION_STRING; }
const char* GetGitDate()    {   return pml::log::GIT_DATE;  }
const char* GetGitTag()     {   return pml::log::GIT_TAG;   }
const char* GetGitBranch()  {   return pml::log::GIT_BRANCH;}

pml::log::Stream pmlLog(pml::enumLevel elevel, const std::string& sPrefix)
{
    return pml::log::Stream(elevel, sPrefix);
}


namespace pml::log
{
    const std::string Stream::STR_LEVEL[6] = {"TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};



Stream log(Level level, const std::string& sPrefix)
{
    return Stream(level, sPrefix);
}



Manager& Manager::Get()
{
    static Manager lm;
    return lm;

}


Manager::Manager() : m_nOutputIdGenerator(0),
m_pThread(std::make_unique<std::thread>([this]{Loop();}))
{

}

Manager::~Manager()
{
    Stop();
}

void Manager::Stop()
{
    if(m_pThread)
    {
        m_bRun = false;
        m_pThread->join();
        m_pThread = nullptr;
    }

}

void Manager::Flush(const std::stringstream& ssLog, Level level, const std::string& sPrefix)
{
    std::scoped_lock lg(m_mutex);
    m_qLog.push(logEntry(ssLog.str(), level,sPrefix));
}

void Manager::Loop()
{
    while(m_bRun)
    {
        HandleQueue();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    HandleQueue();
}

void Manager::HandleQueue()
{
    std::scoped_lock lg(m_mutex);
    while(m_qLog.empty() == false)
    {
        for(auto& pairOutput : m_mOutput)
        {
            pairOutput.second->Flush(m_qLog.front().level, m_qLog.front().sLog, m_qLog.front().sPrefix);
        }
        m_qLog.pop();
    }
}


void Manager::SetOutputLevel(size_t nIndex, Level level)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto itOutput = m_mOutput.find(nIndex);
    if(itOutput != m_mOutput.end())
    {
        itOutput->second->SetOutputLevel(level);
    }
}

void Manager::SetOutputLevel(Level level)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    for(auto& pairOutput : m_mOutput)
    {
        pairOutput.second->SetOutputLevel(level);
    }
}


size_t Manager::AddOutput(std::unique_ptr<Output> pLogout)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ++m_nOutputIdGenerator;
    return m_mOutput.insert(std::make_pair(m_nOutputIdGenerator, move(pLogout))).first->first;
}

void Manager::RemoveOutput(size_t nIndex)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    m_mOutput.erase(nIndex);
}

/******* Output ********/

void Output::Flush(Level level, const std::string&  sLog, const std::string& sPrefix)
{
    if(level >= m_level)
    {
        std::cout << Timestamp().str();
        std::cout << Stream::STR_LEVEL[static_cast<int>(level)] << "\t" << "[" << sPrefix << "]\t" << sLog;
    }
}

std::stringstream Output::Timestamp()
{
    std::stringstream ssTime;
    if(m_nTimestamp != kTsNone)
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        if((m_nTimestamp & kTsDate))
        {
            ssTime << std::put_time(localtime(&in_time_t), "%Y-%m-%d ");
        }
        if((m_nTimestamp & kTsTime))
        {
             ssTime << std::put_time(localtime(&in_time_t), "%H:%M:%S");
        }
        switch(m_resolution)
        {
            case TS::kMillisecond:
                ssTime << "." << std::setw(3) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()%1000);
                break;
            case TS::kMicrosecond:
                ssTime << "." << std::setw(6) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count()%1000000);
                break;
            case TS::kNanosecond:
                ssTime << "." << std::setw(9) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count()%1000000000);
                break;
            default:
                break;
        }
        ssTime << "\t";
    }
    return ssTime;
}

void Output::SetOutputLevel(Level level)
{
    m_level = level;
}

Level Output::GetOutputLevel() const
{
    return m_level;
}


/************* Stream ********************/


Stream::Stream(pml::enumLevel eLevel, const std::string& sPrefix) : m_level(static_cast<Level>(eLevel)), m_sPrefix(sPrefix)
{
}

Stream::Stream(Level level, const std::string& sPrefix) : m_level(level), m_sPrefix(sPrefix)
{

}

Stream::~Stream()
{
    m_stream << "\n";
    flush();
}

Stream::Stream(const Stream& lg) : m_level(lg.GetLevel()), m_sPrefix(lg.GetPrefix())
{
    m_stream << lg.GetStream().rdbuf();
}


Stream& Stream::operator=(const Stream& lg)
{
    if(&lg != this)
    {
        m_stream.str(std::string());
        m_stream.clear();

        m_stream << lg.GetStream().rdbuf();
        m_level = lg.GetLevel();
        m_sPrefix = lg.GetPrefix();
    }
    return *this;
}


void Stream::flush()
{
    Manager::Get().Flush(m_stream, m_level, m_sPrefix);

    m_stream.str(std::string());
    m_stream.clear();
}

void Stream::SetOutputLevel(size_t nIndex, pml::enumLevel eLevel)
{
    Manager::Get().SetOutputLevel(nIndex, static_cast<Level>(eLevel));
}

void Stream::SetOutputLevel(pml::enumLevel eLevel)
{
    Manager::Get().SetOutputLevel(static_cast<Level>(eLevel));
}

void Stream::SetOutputLevel(size_t nIndex, Level level)
{
    Manager::Get().SetOutputLevel(nIndex, level);
}

void Stream::SetOutputLevel(Level level)
{
    Manager::Get().SetOutputLevel(level);
}

size_t Stream::AddOutput(std::unique_ptr<Output> pLogout)
{
    return Manager::Get().AddOutput(std::move(pLogout));
}

void Stream::RemoveOutput(size_t nIndex)
{
    Manager::Get().RemoveOutput(nIndex);
}



Stream& Stream::operator<<(ManipFn manip) /// endl, flush, setw, setfill, etc.
{
    manip(m_stream);

    if (manip == static_cast<ManipFn>(std::flush)
     || manip == static_cast<ManipFn>(std::endl ) )
        this->flush();

    return *this;
}

Stream& Stream::operator<<(FlagsFn manip) /// setiosflags, resetiosflags
{
    manip(m_stream);
    return *this;
}

Stream& Stream::operator()(Level level)
{
    m_level = level;
    return *this;
}

Stream& Stream::SetLevel(Level level)
{
    m_level = level;
    return *this;
}

void Stream::Stop()
{
    Manager::Get().Stop();
}
}
