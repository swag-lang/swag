#global if WINDOWS
#global public
#global namespace time
using win32

private var g_perfCounterFreq: u64

#init
{
    QueryPerformanceFrequency(acast &g_perfCounterFreq)
}

// Get current time precise value
func nowPrecise()->u64
{
    var counter: u64 = ?
    QueryPerformanceCounter(acast &counter)
    return counter
}

// The frequency of the precise counter, in ticks per second
func preciseFrequency()->u64
{
    return g_perfCounterFreq
}
