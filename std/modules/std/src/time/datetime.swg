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

public impl DateTime
{
    private const DaysToMonth365 = @[0'u16, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365]
    private const DaysToMonth366 = @[0'u16, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366]

    func opEquals(self, other: Self)->bool
    {
        return memory.compare(self, other, @sizeof(Self)) == 0
    }

    // Returns true if this is a valid datatime
    func isValid(using self)->bool
    {
        if year == 0 || year > 9999 return false
        if month == 0 || month > 12 return false
        return true
    }

    // Compute the ticks part of the datatime
    func toTicks(using self)
    {
        debug.assert(isValid())
    }

    // Returns true if the given year is a leap year
    func isLeapYear(year: u16)->bool
    {
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)
    }

    // Returns the numober of days of the given month
    func daysInMonth(year, month: u16)->u16
    {
        //debug.assert(1 <= month <= 12)
        //if isLeapYear(year)
        //    return acast (DaysToMonth366[month] - DaysToMonth366[month - 1])
        return acast (DaysToMonth365[month] - DaysToMonth365[month - 1])
    }
}
