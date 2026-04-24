#include "logtofile.h"
#ifdef _WIN32
#include <direct.h>   // _mkdir
#endif
#include <iostream>
#include <string>
#include <sys/stat.h> // stat
#include <errno.h>    // errno, ENOENT, EEXIST

#include <chrono>
#include <iomanip>
#include <algorithm>

namespace pml::log
{
#if ((defined(_MSVC_LANG) && _MSVC_LANG >=201703L) || __cplusplus >= 201703L)

File::File(const std::filesystem::path& rootPath,int nTimestamp, Output::TS resolution, bool bLocalTime) : Output(nTimestamp, resolution),
m_rootPath(rootPath),
m_bLocalTime(bLocalTime)
{
}

void File::OpenFile(const std::string& sFileName)
{
    if(m_ofLog.is_open())
    {
        m_ofLog.close();
    }


    m_sCurrentFile = sFileName;
    auto sPath = m_rootPath.string() + sFileName+".log";

    if(std::error_code ec; std::filesystem::create_directories(m_rootPath, ec) == false && ec.value() !=0)
    {
        m_bOk = false;
        std::cout << "Could not create log directory " << m_rootPath << "\t" << ec.message() << std::endl;
    }
    else
    {
        m_bOk = true;
        m_ofLog.open(sPath, std::fstream::app);
    }
}

void File::DoOutputMessage(Level level, const std::string&  sLog, const std::string& sPrefix)
{
    if(level >= m_level)// && m_bOk)
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ssFileName;
        if(m_bLocalTime)
        {
            ssFileName << std::put_time(localtime(&in_time_t), "/%Y-%m-%dT%H");
        }
        else
        {
            ssFileName << std::put_time(gmtime(&in_time_t), "/%Y-%m-%dT%H");
        }

        if(m_ofLog.is_open() == false || ssFileName.str() != m_sCurrentFile)
        {
            OpenFile(ssFileName.str());
        }

        if(m_ofLog.is_open())
        {
            m_ofLog << Timestamp().str();
            m_ofLog << Stream::STR_LEVEL[static_cast<int>(level)] << "\t" << "[" << sPrefix << "]\t" << sLog;
            m_ofLog.flush();
        }
        else
        {
            if(m_bLocalTime)
            {
                std::cout << Stream::STR_LEVEL[static_cast<int>(level)] << "\t" << "[" << sPrefix << "]\t" << sLog;
            }
            else
            {
                std::cout << Stream::STR_LEVEL[static_cast<int>(level)] << "\t" << "[" << sPrefix << "]\t" << sLog;
            }
            std::cout.flush();
        }
    }
}

void File::Flush()
{
    if(m_ofLog.is_open())
    {
        m_ofLog.flush();
    }
    else
    {
        std::cout << std::flush;
    }   
}

#else
bool isDirExist(const std::string& path)
{
#ifdef _WIN32
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & _S_IFDIR) != 0;
#else
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
#endif
}

bool makePath(const std::string& path)
{
#ifdef _WIN32
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
#endif
    if (ret == 0)
    {
        return true;
    }

    switch (errno)
    {
    case ENOENT:
        // parent didn't exist, try to create it
        {
            auto pos = path.find_last_of('/');
            if (pos == std::string::npos)
	    {
#ifdef _WIN32
                pos = path.find_last_of('\\');
	    }
            if (pos == std::string::npos)
	    {
#endif
                return false;
	    }
            if (!makePath( path.substr(0, pos) ))
	    {
                return false;
	    }
        }
        // now, try to create again
#ifdef _WIN32
        return 0 == _mkdir(path.c_str());
#else
        return 0 == mkdir(path.c_str(), mode);
#endif

    case EEXIST:
        // done!
        return isDirExist(path);

    default:
        return false;
    }
}


std::string CreatePath(std::string sPath)
{
    //make sure path doesn;t end in a /
    if(sPath[sPath.length()-1] == '/' || sPath[sPath.length()-1] == '\\')
    {
        sPath = sPath.substr(0, sPath.length()-1);
    }
    return sPath;
}



File::File(const std::string& sRootPath,int nTimestamp, Output::TS resolution, bool bLocalTime) : Output(nTimestamp, resolution),
m_sRootPath(CreatePath(sRootPath)),
m_bLocalTime(bLocalTime)
{
}

void File::OpenFile(const std::string& sFilePath, const std::string& sFileName)
{
    if(m_ofLog.is_open())
    {
        m_ofLog.close();
    }

    m_sFilePath = sFilePath;
    m_sFileName = sFileName;


    if(makePath(m_sFilePath) == false)
    {
        std::cout << "Unable to create log file "  << sFilePath << std::endl;
    }
    auto sFile = m_sFilePath+m_sFileName;
    m_ofLog.open(sFile, std::fstream::app);

    //chande the permissions
    //chmod(sFile.c_str(), 0664);
}

void File::DoOutputMessage(Level level, const std::string&  sLog, const std::string& sPrefix)
{
    if(level >= m_level)// && m_bOk)
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ssFileName;
        if(m_bLocalTime)
        {
            ssFileName << std::put_time(localtime(&in_time_t), "/%Y-%m-%dT%H");
        }
        else
        {
            ssFileName << std::put_time(gmtime(&in_time_t), "/%Y-%m-%dT%H");
        }

        if(m_ofLog.is_open() == false || ssFileName.str() != m_sFileName)
        {
            OpenFile(m_sRootPath, ssFileName.str());
        }

        if(m_ofLog.is_open())
        {
            m_ofLog << Timestamp().str();
            m_ofLog << Stream::STR_LEVEL[static_cast<int>(level)] << "\t" << "[" << sPrefix << "]\t" << sLog;
            m_ofLog.flush();
        }
        else
        {
            if(m_bLocalTime)
            {
                std::cout << Stream::STR_LEVEL[static_cast<int>(level)] << "\t" << "[" << sPrefix << "]\t" << sLog;
            }
            else
            {
                std::cout << Stream::STR_LEVEL[static_cast<int>(level)] << "\t" << "[" << sPrefix << "]\t" << sLog;
            }
            std::cout.flush();
        }
    }
}

void File::Flush()
{
    if(m_ofLog.is_open())
    {
        m_ofLog.flush();
    }
    else
    {
        std::cout << std::flush;
    }   
}

#endif

}
