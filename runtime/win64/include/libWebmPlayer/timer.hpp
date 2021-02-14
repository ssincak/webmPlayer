#ifndef _UVPX_TIMER_H_
#define _UVPX_TIMER_H_

#include <atomic>

#ifdef _MSC_VER
#define NOMINMAX
#include <Windows.h>
#else
#include <chrono>
#endif

namespace uvpx
{
    class Timer
    {
    protected:

        std::atomic<bool> m_active;

#ifdef _MSC_VER
        typedef LARGE_INTEGER TimePoint;
#else
        typedef std::chrono::steady_clock::time_point TimePoint;
#endif

        TimePoint m_start;
        TimePoint m_pauseStart;

        double  m_pauseDuration;

        void  now(TimePoint& tp);
        double secondsElapsed(const TimePoint& start, const TimePoint& end);

    public:
        Timer();
        ~Timer();

        void  start();
        void  stop();
        void  pause();
        void  resume();

        double elapsedSeconds();
    };
}
#endif  // _UVPX_TIMER_H_
