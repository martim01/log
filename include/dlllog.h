#pragma once

#ifdef _WIN32
    #ifdef LOG_DLL
        #define LOG_EXPORT __declspec(dllexport)
    #else
        #define LOG_EXPORT __declspec(dllimport)
    #endif // NMOS_EXPORT
#else
    #define LOG_EXPORT
#endif
