#pragma once

#ifndef PERFORMANCE_STOPWATCH_H
#define PERFORMANCE_STOPWATCH_H

#include <iostream>

/*******************************************************************************
 *  class Stopwatch -- simple stopwatch timer for performance optimization
 *
 *  Stopwatch prints out elapsed time information for measuring	the run time
 *  of long-running tasks. Stopwatch can also return the elapsed time without
 *  printing anything.
 *
 *  Stopwatch is meant to be used with the RAII idiom. In its
 *  simplest use, the block
 *      {
 *          Stopwatch sw("TheThing()");
 *          sw.Show("Just after initialized");
 *          TheThing();
 *      }
 *  produces two lines of output like
 *      TheThing(): start
 *      TheThing(): Just after initialized at 3mS
 *      // anything printed by TheThing()
 *      TheThing(): stop 63mS
 *
 *  If you prefer millisec then include logger/Stopwatchmsec.h in your code or 
 *  for microsec accuracy then include logger/Stopwatchmicro.h. Both uses chrono
 *  to measure time so if you find other ways to measure time, you can easily extend it.  
 *
 *  If you want Stopwatch print its measurements directly to Log,  then provide
 *  a nonempty activity while constructing. However if logging seem to take significant
 *  you can use "" empty activity so that you collect measurement values from return
 *  value and then log by yourself in the code all at once.
 *
 *  What StopWatch logs in detail:
 *      ctor():							"Stopwatch: start"
 *      ctor(false):					nothing printed
 *      Show():							"Stopwatch: show at xxxx mS"
 *                                      sets lap time. Get with LapGet()
 *      Stop():							"Stopwatch: stop xxxx mS"
 *                                      sets lap time. Get with LapGet()
 *      Stop() when not running:		"Stopwatch: not started"
 *      Start():						"Stopwatch: start"
 *                                      clears lap time
 *      Start() when running:			"Stopwatch: start xxxx mS"
 *                                      sets lap time. Get with LapGet()
 *  What it prints when activity name is specified
 *      ctor("activity"):				"activity: start"
 *      Show("theEvent"):				"activity: theEvent at xxxx mS"
 *      Stop("theEvent"):				"activity: theEvent xxxx mS"
 *                                      sets lap time. Get with LapGet()
 *      Stop("theEvent") not running:	"activity: not started"
 *      Start("theEvent") running:		"activity: theEvent xxxx mS"
 *                                      sets lap time. Get with LapGet()
 *      Start("theEvent") not running: "activity: theEvent"
 *                                      clears lap time
 *
 *  printing can be suppressed 
 *      ctor("") or ctor(nullptr)		prints nothing
 *      Show("") or Show(nullptr)		prints nothing, sets lap time
 *      Start("") or Start(nullptr):		prints nothing, sets lap time if running
 *      Stop("") or Stop(nullptr):		sets lap time. Get with LapGet()
 *
 ********************************************************************************/


template <typename T> class basic_stopwatch : public T {
public:
    typedef T BaseTimer;
    typedef unsigned long tick_t;

    // create, optionally start timing an activity
    explicit basic_stopwatch(bool start);
    explicit basic_stopwatch(char const* activity = "Stopwatch",
                             bool start=true);
    basic_stopwatch(std::ostream& log,
                    char const* activity="Stopwatch", 
                    bool start=true); 

    // stop and destroy a stopwatch
    ~basic_stopwatch();

    // get last lap time (time of last stop)
    tick_t LapGet() const;

    // predicate: return true if the stopwatch is running
    bool IsStarted() const;

    // show accumulated time, keep running, set/return lap
    tick_t Show(char const* event="show");

    // (re)start a stopwatch, set/return lap time
    tick_t Start(char const* event_namee="start");

    // stop a running stopwatch, set/return lap time
    tick_t Stop(char const* event_name="stop");

private:    //  members
    char const*     m_activity; 	// "activity" string
    tick_t          m_lap;		// lap time (time of last stop or 0)
    std::ostream&   m_log;		// stream on which to log events
};

//  performs a Start() if start_now == true
template <typename T> inline basic_stopwatch<T>::basic_stopwatch(bool start_now)
  : m_activity("Stopwatch")
  , m_lap(0)
  , m_log(std::cout) 
{
    if (start_now)
        Start();
}

//	performs a start if start_now == true, suppress print by ctor("")
template <typename T> inline basic_stopwatch<T>::basic_stopwatch(char const* activity, bool start_now)
  : m_activity(activity && activity[0] ? activity : nullptr)
  , m_lap(0)
  , m_log(std::cout) 
{
    if (start_now) {
        if (m_activity)
            Start();
        else
            Start(nullptr);
    }
}

//	set log output, optional printout, optional start
template <typename T> inline basic_stopwatch<T>::basic_stopwatch(std::ostream& log, char const* activity, bool start_now)
  : m_activity(activity && activity[0] ? activity : nullptr)
  , m_lap(0)
  , m_log(log) 
{
    if (start_now) {
        if (m_activity)
            Start();
        else
            Start(nullptr);
    }
}

//	stop/destroy stopwatch, print message if activity was set in ctor
template <typename T> inline basic_stopwatch<T>::~basic_stopwatch() {
    if (IsStarted()) {
        if (m_activity)
            Stop();
        else
            Stop(nullptr);
    }
}

//   predicate: return true if the stopwatch is running
template <typename T> inline bool basic_stopwatch<T>::IsStarted() const
{
    return BaseTimer::IsStarted();
}

//	get the last lap time (time of last stop)
template <typename T> inline typename basic_stopwatch<T>::tick_t basic_stopwatch<T>::LapGet() const
{
    return m_lap;
}

//   show accumulated time, keep running, get/return lap time
template <typename T> inline typename basic_stopwatch<T>::tick_t basic_stopwatch<T>::Show(char const* event_name) {
    if (IsStarted()) {
        m_lap = BaseTimer::GetMs();
        if (event_name && event_name[0]) {
            if (m_activity)
                m_log << m_activity << ": " << event_name << " at " << m_lap << "mS" << std::endl << std::flush;
        }
    }
    else {
        if (m_activity)
            m_log << m_activity << ": not started" << std::endl << std::flush;
    }
    return m_lap;
}

//   (re)start a stopwatch, set/return lap time
template <typename T> inline typename basic_stopwatch<T>::tick_t basic_stopwatch<T>::Start(char const* event_name) {
    if (IsStarted()) {
        Stop(event_name);
    }
    else {
        if (event_name && event_name[0]) {
            if (m_activity)
                m_log << m_activity << ": " << event_name << std::endl << std::flush;
        }
    }
    BaseTimer::Start();
    return m_lap;
}

//   stop a running stopwatch and print the accumulated time
template <typename T> inline typename basic_stopwatch<T>::tick_t basic_stopwatch<T>::Stop(char const* event_name) {
    if (IsStarted()) {
        m_lap = BaseTimer::GetMs();
        if (event_name && event_name[0]) {
            if (m_activity)
                m_log << m_activity << ": " << event_name << " " << m_lap << "mS" << std::endl << std::flush;
        }
    }
    BaseTimer::Clear();
    return m_lap;
}

template <typename clock_, typename resolution>
class TimerBaseChrono {

public:
        //      clears the timer
        TimerBaseChrono() : m_start(clock_::time_point::min()) { }

        //  clears the timer
        void Clear() { 
                m_start = clock_::time_point::min(); 
        }

        //      returns true if the timer is running
        bool IsStarted() const {
                return (m_start != clock_::time_point::min());
        }

        //      start the timer
        void Start()            { m_start = clock_::now(); }

        //      get the period since the timer was started
        unsigned long GetMs() {
                if (IsStarted()) {
                        return (unsigned long)(std::chrono::duration_cast<resolution>(clock_::now() - m_start).count());
                }
                return 0;
        }
private:
        std::chrono::system_clock::time_point m_start;
};

# endif
