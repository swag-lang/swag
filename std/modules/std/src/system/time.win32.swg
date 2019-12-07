#if swag.WINDOWS
{
	private var g_perfCounterFreq: u64

	#init
	{
		std.os.QueryPerformanceFrequency(acast &g_perfCounterFreq)
	}

	namespace std.time
	{
		using os
		public
		{
			func nowMs()->u32
			{
				return 0
			}

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
}