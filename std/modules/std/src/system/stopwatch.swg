namespace std.time
{
    // Provides a set of methods and properties that you can use to accurately measure elapsed time
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
            // Starts, or resumes, measuring elapsed time for an interval
			func start(using self)
			{
				if !isRunning
				{
					isRunning = true
					startTimeStamp = nowTicks()
				}
			}

            // Stops measuring elapsed time for an interval
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

            // Stops time interval measurement and resets the elapsed time to zero
			func reset(using self)
			{
				elapsedTicks = 0
				startTimeStamp = 0
				isRunning = false
			}

            // Stops time interval measurement, resets the elapsed time to zero, and starts measuring elapsed time
			func restart(using self)
			{
				elapsedTicks = 0
				startTimeStamp = nowTicks()
				isRunning = true
			}

            // Gets the total elapsed time in milliseconds measured by the current instance
			func elapsedMilliseconds(using self)->u32
			{
				return acast ((elapsedTicks * 1000) / ticksFrequency())
			}

            // Gets the total elapsed time in microseconds measured by the current instance
			func elapsedMicroseconds(using self)->u32
			{
				return acast ((elapsedTicks * 1000000) / ticksFrequency())
			}
		}
	}
}
