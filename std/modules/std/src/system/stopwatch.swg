namespace std.time
{
	public struct Stopwatch
	{    		
		readonly isRunning:			bool
		readonly startTimeStamp:	u64
		readonly elapsedTicks:		u64
	}

	impl Stopwatch
	{
		public
		{
			func start(using self)
			{
				if !isRunning
				{
					isRunning = true
					startTimeStamp = nowTicks()
				}
			}

			func stop(using self)
			{
				if isRunning
				{
					endTimeStamp := nowTicks()
					elapsedTicksPeriod := endTimeStamp - startTimeStamp
					elapsedTicks += elapsedTicksPeriod
					isRunning = false
				}
			}

			func reset(using self)
			{
				elapsedTicks = 0
				startTimeStamp = 0
				isRunning = false
			}

			func restart(using self)
			{
				elapsedTicks = 0
				startTimeStamp = nowTicks()
				isRunning = true
			}

			func elapsedMilliseconds(using self)->u32
			{
				return acast ((elapsedTicks * 1000) / ticksFrequency())
			}

			func elapsedMicroseconds(using self)->u32
			{
				return acast ((elapsedTicks * 1000000) / ticksFrequency())
			}
		}
	}
}
