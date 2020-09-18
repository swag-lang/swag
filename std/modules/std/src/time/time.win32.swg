#if std.WINDOWS
{
    using win32

	private var g_perfCounterFreq: u64

	#init
	{
		QueryPerformanceFrequency(acast &g_perfCounterFreq)
	}

	namespace std.time
	{
        public func nowTicks()->u64
        {
            var counter: u64 = ?
            QueryPerformanceCounter(acast &counter)
            return counter
        }

        public func ticksFrequency()->u64
        {
            return g_perfCounterFreq
        }
	}
}