#include "log.h"
#include "logmanager.h"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace pml;

const std::string LogStream::STR_LEVEL[6] = {"TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};


LogStream pmlLog(enumLevel elevel)
{
    LogStream lg(elevel);
    return lg;
}

//LogStream Log(enumLevel elevel)
//{
//    LogStream lg(elevel);
//    return lg;
//}

LogManager& LogManager::Get()
{
    static LogManager lm;
    return lm;

}

LogManager::LogManager() : m_nOutputIdGenerator(0)
{
}

void LogManager::Flush(const std::stringstream& ssLog, enumLevel eLevel)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    for(auto& pairOutput : m_mOutput)
    {
        pairOutput.second->Flush(eLevel, ssLog);
    }
}


void LogManager::SetOutputLevel(size_t nIndex, enumLevel eLevel)
{
    auto itOutput = m_mOutput.find(nIndex);
    if(itOutput != m_mOutput.end())
    {
        itOutput->second->SetOutputLevel(eLevel);
    }
}

void LogManager::SetOutputLevel(enumLevel eLevel)
{
    for(auto& pairOutput : m_mOutput)
    {
        pairOutput.second->SetOutputLevel(eLevel);
    }
}


size_t LogManager::AddOutput(std::unique_ptr<LogOutput> pLogout)
{
    ++m_nOutputIdGenerator;
    return m_mOutput.insert(std::make_pair(m_nOutputIdGenerator, move(pLogout))).first->first;
}

void LogManager::RemoveOutput(size_t nIndex)
{
    m_mOutput.erase(nIndex);
}



LogStream::LogStream(enumLevel eLevel) : m_logLevel(eLevel)
{

}

LogStream::~LogStream()
{
    m_stream << "\n";
    flush();
}

LogStream::LogStream(const LogStream& lg) : m_logLevel(lg.GetLevel())
{
    m_stream << lg.GetStream().rdbuf();
}


LogStream& LogStream::operator=(const LogStream& lg)
{
    if(&lg != this)
    {
        m_stream.str(std::string());
        m_stream.clear();

        m_stream << lg.GetStream().rdbuf();
        m_logLevel = lg.GetLevel();
    }
    return *this;
}


void LogStream::flush()
{
    LogManager::Get().Flush(m_stream, m_logLevel);


    m_stream.str(std::string());
    m_stream.clear();
}

void LogStream::SetOutputLevel(size_t nIndex, enumLevel eLevel)
{
    LogManager::Get().SetOutputLevel(nIndex, eLevel);
}

void LogStream::SetOutputLevel(enumLevel eLevel)
{
    LogManager::Get().SetOutputLevel(eLevel);
}


size_t LogStream::AddOutput(std::unique_ptr<LogOutput> pLogout)
{
    return LogManager::Get().AddOutput(std::move(pLogout));
}

void LogStream::RemoveOutput(size_t nIndex)
{
    LogManager::Get().RemoveOutput(nIndex);
}



LogStream& LogStream::operator<<(ManipFn manip) /// endl, flush, setw, setfill, etc.
{
    manip(m_stream);

    if (manip == static_cast<ManipFn>(std::flush)
     || manip == static_cast<ManipFn>(std::endl ) )
        this->flush();

    return *this;
}

LogStream& LogStream::operator<<(FlagsFn manip) /// setiosflags, resetiosflags
{
    manip(m_stream);
    return *this;
}

LogStream& LogStream::operator()(enumLevel e)
{
    m_logLevel = e;
    return *this;
}

void LogStream::SetLevel(enumLevel e)
{
    m_logLevel = e;
}



void LogOutput::Flush(enumLevel eLogLevel, const std::stringstream&  logStream)
{
    if(eLogLevel >= m_eLevel)
    {
        std::cout << Timestamp().str();
        std::cout << LogStream::STR_LEVEL[eLogLevel] << "\t" << logStream.str();
    }
}

std::stringstream LogOutput::Timestamp()
{
    std::stringstream ssTime;
    if(m_nTimestamp != TS_NONE)
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        if((m_nTimestamp & TS_DATE))
        {
            ssTime << std::put_time(localtime(&in_time_t), "%Y-%m-%d ");
        }
        if((m_nTimestamp & TS_TIME))
        {
             ssTime << std::put_time(localtime(&in_time_t), "%H:%M:%S");
        }
        switch(m_eResolution)
        {
            case TSR_MILLISECOND:
                ssTime << "." << std::setw(3) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()%1000);
                break;
            case TSR_MICROSECOND:
                ssTime << "." << std::setw(6) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count()%1000000);
                break;
            case TSR_NANOSECOND:
                ssTime << "." << std::setw(9) << std::setfill('0') << (std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count()%1000000000);
                break;
            default:
                break;
        }
        ssTime << "\t";
    }
    return ssTime;
}

void LogOutput::SetOutputLevel(enumLevel eLevel)
{
    m_eLevel = eLevel;
}

enumLevel LogOutput::GetOutputLevel() const
{
    return m_eLevel;
}
