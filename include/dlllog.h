#pragma once

#ifdef __GNUWIN32__
    #ifdef LOG_DLL
        #define LOG_EXPORT __declspec(dllexport)
    #else
        #define LOG_EXPORT __declspec(dllimport)
    #endif // NMOS_EXPORT
#else
    #define LOG_EXPORT
#endif
