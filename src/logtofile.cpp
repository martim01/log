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

using namespace pml;

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



LogToFile::LogToFile(const std::string& sRootPath,int nTimestamp, pml::LogOutput::enumTS eResolution) : LogOutput(nTimestamp, eResolution),
m_sRootPath(CreatePath(sRootPath))
{
}

void LogToFile::OpenFile(const std::string& sFilePath, const std::string& sFileName)
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

void LogToFile::Flush(pml::enumLevel eLogLevel, const std::stringstream&  logStream)
{
    if(eLogLevel >= m_eLevel)
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ssFilePath;
        std::stringstream ssFileName;
        ssFileName << std::put_time(localtime(&in_time_t), "/%Y-%m-%dT%H") << ".log";

        if(m_ofLog.is_open() == false || m_sRootPath != m_sFilePath || ssFileName.str() != m_sFileName)
        {
            OpenFile(m_sRootPath, ssFileName.str());
        }

        if(m_ofLog.is_open())
        {
            m_ofLog << Timestamp().str();
            m_ofLog << pml::LogStream::STR_LEVEL[eLogLevel] << "\t" << logStream.str();
            m_ofLog.flush();
        }
    }
}



