namespace std.time
{
    /// Represents an instant in time, typically expressed as a date and time of day
	public struct DateTime
	{
        /// The day of the month
		day:			u16
        /// The day of the week
		dayOfWeek:		u16
        /// The month
		month:			u16
        /// The year
		year:			u16
        /// The hour (0-24)
		hour:			u16
        /// The minutes
		minute:			u16
        /// The seconds
		second:			u16
        /// The milliseconds
		millisecond:	u16
	}

	impl DateTime
	{
		public func opEquals(self, other: Self)->bool
		{
			return @memcmp(self, other, @sizeof(Self)) == 0
		}
	}
}
