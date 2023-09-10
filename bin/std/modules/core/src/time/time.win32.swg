#global #if WINDOWS
#global namespace Time
using Win32

var g_PerfCounterFreq: u64

#init
{
    QueryPerformanceFrequency(acast &g_PerfCounterFreq)
}

// Get current time precise value
public func nowPrecise()->u64
{
    var counter: u64 = undefined
    QueryPerformanceCounter(acast &counter)
    return counter
}

// The frequency of the precise counter, in ticks per second
public func preciseFrequency()->u64
{
    return g_PerfCounterFreq
}
