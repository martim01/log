#include "log.h"
#include <iostream>

using namespace pml;

const std::string Log::STR_LEVEL[6] = {"TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};

Log& Log::Get(enumLevel eLevel)
{
    static Log lg;
    lg.SetLevel(eLevel);
    return lg;
}


void LogOutput::Flush(int nLogLevel, const std::stringstream&  logStream)
{
    if(nLogLevel >= m_eLevel)
    {
        std::cout << Log::STR_LEVEL[nLogLevel] << "\t" << logStream.str();
    }
}

void LogOutput::SetOutputLevel(Log::enumLevel eLevel)
{
    m_eLevel = eLevel;
}

Log::enumLevel LogOutput::GetOutputLevel() const
{
    return m_eLevel;
}

Log::Log() : m_logLevel(LOG_INFO) , m_nOutputIdGenerator(0)
{

}


void Log::flush()
{
    for(auto itOutput  = m_mOutput.begin(); itOutput != m_mOutput.end(); ++itOutput)
    {
        itOutput->second->Flush(m_logLevel, m_stream);
    }

    m_stream.str(std::string());
    m_stream.clear();
}


void Log::SetOutputLevel(size_t nIndex, enumLevel eLevel)
{
    auto itOutput = m_mOutput.find(nIndex);
    if(itOutput != m_mOutput.end())
    {
        itOutput->second->SetOutputLevel(eLevel);
    }
}

void Log::SetOutputLevel(enumLevel eLevel)
{
    for(auto itOutput  = m_mOutput.begin(); itOutput != m_mOutput.end(); ++itOutput)
    {
        itOutput->second->SetOutputLevel(eLevel);
    }
}


size_t Log::AddOutput(std::unique_ptr<LogOutput> pLogout)
{
    ++m_nOutputIdGenerator;
    return m_mOutput.insert(std::make_pair(m_nOutputIdGenerator, move(pLogout))).first->first;
}

void Log::RemoveOutput(size_t nIndex)
{
    m_mOutput.erase(nIndex);
}




Log& Log::operator<<(ManipFn manip) /// endl, flush, setw, setfill, etc.
{
    manip(m_stream);

    if (manip == static_cast<ManipFn>(std::flush)
     || manip == static_cast<ManipFn>(std::endl ) )
        this->flush();

    return *this;
}

Log& Log::operator<<(FlagsFn manip) /// setiosflags, resetiosflags
{
    manip(m_stream);
    return *this;
}

Log& Log::operator()(enumLevel e)
{
    m_logLevel = e;
    return *this;
}

void Log::SetLevel(enumLevel e)
{
    m_logLevel = e;
}
