namespace std.time;

// Represents an instant in time, typically expressed as a date and time of day
public struct DateTime
{
    day:			u16
    dayOfWeek:		u16
    month:			u16
    year:			u16
    hour:			u16
    minute:			u16
    second:			u16
    millisecond:	u16
}

impl DateTime
{
    public func opEquals(self, other: Self)->bool
    {
        return std.memory.compare(self, other, @sizeof(Self)) == 0
    }
}
