#if std.WINDOWS
{
    using win32

	private var g_perfCounterFreq: u64

	#init
	{
		QueryPerformanceFrequency(acast &g_perfCounterFreq)
	}

	public namespace std.time
	{
        func nowTicks()->u64
        {
            var counter: u64 = ?
            QueryPerformanceCounter(acast &counter)
            return counter
        }

        func ticksFrequency()->u64
        {
            return g_perfCounterFreq
        }
	}
}