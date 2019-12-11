#if swag.WINDOWS
{
	namespace std.time
	{
		using os

		impl DateTime
		{
            /// Initialize the structure with the current local date and time
			public func now(using self)
			{
				var systemTime: SYSTEMTIME = ?
				GetLocalTime(&systemTime)

				using systemTime
				day         = wDay
				dayOfWeek   = wDayOfWeek
				month       = wMonth
				year        = wYear
				hour        = wHour
				minute      = wMinute
				second      = wSecond
				millisecond = wMilliseconds
			}
		}
	}
}
