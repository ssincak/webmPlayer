#include "utils.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <fstream>

// For std::max
#include <algorithm>

#ifdef _WIN32
#if _MSC_VER
#define NOMINMAX
#include <Windows.h>
#endif
#elif defined(__linux__)
#include <unistd.h>
#endif

#include "player.hpp"

namespace uvpx
{
    DebugLogFuncPtr     g_debugLogFuncPtr = nullptr;

    /// Returns number of CPU cores.
    int  getSystemThreadsCount()
    {
#if _MSC_VER
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return std::max(1, (int)sysinfo.dwNumberOfProcessors);
#elif defined(__linux__) || defined(__APPLE__)
        return std::max(1L, sysconf(_SC_NPROCESSORS_ONLN));
#endif

        return 1;
    }

    /// Sets debug log callback.
    void setDebugLog(DebugLogFuncPtr func)
    {
        g_debugLogFuncPtr = func;
    }

    /// Internal debug log.
    void debugLog(const char *format, ...)
    {
        char buffer[UVPX_THREAD_ERROR_DESC_SIZE];

        va_list arglist;
        va_start(arglist, format);
        vsprintf(buffer, format, arglist);
        va_end(arglist);

        if (g_debugLogFuncPtr != nullptr)
            g_debugLogFuncPtr(buffer);
    }
}
