#if std.WINDOWS;
using win32
public namespace std.time;

private var g_perfCounterFreq: u64

#init
{
    QueryPerformanceFrequency(acast &g_perfCounterFreq)
}


func nowPrecise()->u64
{
    var counter: u64 = ?
    QueryPerformanceCounter(acast &counter)
    return counter
}

func preciseFrequency()->u64
{
    return g_perfCounterFreq
}
