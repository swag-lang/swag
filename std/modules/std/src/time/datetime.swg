public namespace std.time;

// Represents an instant in time, typically expressed as a date and time of day
struct DateTime
{
    year:			u16
    month:			u16
    day:			u16
    hour:			u16
    minute:			u16
    second:			u16
    millisecond:	u16
}

impl DateTime
{
    func opEquals(self, other: Self)->bool
    {
        return memory.compare(self, other, @sizeof(Self)) == 0
    }

    // Returns true if this is a valid datatime
    func isValid(using self)->bool
    {
        if year == 0 || year > 9999 return false
        if month == 0 || month > 12 return false
        if hour >= 24 return false
        if minute >= 60 return false
        if second >= 60 return false
        if millisecond >= 1000 return false
        return true
    }
}
