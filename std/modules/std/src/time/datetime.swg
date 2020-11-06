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
    private const TicksPerMillisecond: u64 = 10000
    private const TicksPerSecond: u64 = TicksPerMillisecond * 1000
    private const TicksPerMinute: u64 = TicksPerSecond * 60
    private const TicksPerHour: u64   = TicksPerMinute * 60
    private const TicksPerDay: u64    = TicksPerHour * 24

    private const DaysPerYear     = 365
    private const DaysPer4Years   = (DaysPerYear * 4) + 1
    private const DaysPer100Years = (DaysPer4Years * 25) - 1
    private const DaysPer400Years = (DaysPer100Years * 4) + 1

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
    func dateToTicks(year, month, day: u16)->u64
    {
        debug.assert(1 <= year <= 9999)
        debug.assert(1 <= month <= 12)
        debug.assert(1 <= day <= daysInMonth(year, month))

        days := isLeapYear(year) ? @dataof(DaysToMonth366) : @dataof(DaysToMonth365)
        y := cast(u64) (year - 1)
        n := ((y * 365) + (y / 4) + (y / 400) + days[month - 1] + (day - 1)) - (y / 100)
/*
console.print("n0: %\n", n)
console.print("n1: %\n", n * TicksPerDay)
console.print("n2: %\n", TicksPerDay)
*/
        return n * TicksPerDay
    }

    func ticksToDate(ticks: u64) -> {year: u16, month: u16, day: u16}
    {
        // n = number of days since 1/1/0001
        n := ticks / TicksPerDay

        // y400 = number of whole 400-year periods since 1/1/0001
        y400 := n / DaysPer400Years
        // n = day number within 400-year period
        n -= y400 * DaysPer400Years
        // y100 = number of whole 100-year periods within 400-year period
        y100 := n / DaysPer100Years
        // Last 100-year period has an extra day, so decrement result if 4
        if y100 == 4 y100 = 3
        // n = day number within 100-year period
        n -= y100 * DaysPer100Years
        // y4 = number of whole 4-year periods within 100-year period
        y4 := n / DaysPer4Years
        // n = day number within 4-year period
        n -= y4 * DaysPer4Years
        // y1 = number of whole years within 4-year period
        y1 := n / DaysPerYear
        // Last year has an extra day, so decrement result if 4
        if y1 == 4 y1 = 3
        // compute year
        year := (y400 * 400) + (y100 * 100) + (y4 * 4) + (y1 + 1)
        // n = day number within year
        n -= y1 * DaysPerYear
        // dayOfYear = n + 1;
        // Leap year calculation looks different from IsLeapYear since y1, y4,
        // and y100 are relative to year 1, not year 0
        leapYear := y1 == 3 && (y4 != 24 || y100 == 3)
        days := leapYear ? @dataof(DaysToMonth366) : @dataof(DaysToMonth365)
        // All months have less than 32 days, so n >> 5 is a good conservative
        // estimate for the month
        m := (n >> 5) + 1
        // m = 1-based month number
        while (n >= days[m]) m += 1
        // Compute month and day
        month := m;
        day := (n - days[m - 1]) + 1

        return @{cast(u16) year, cast(u16) month, cast(u16) day}
    }

    // Returns true if the given year is a leap year
    func isLeapYear(year: u16)->bool
    {
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)
    }

    // Returns the numober of days of the given month, for the given year
    func daysInMonth(year, month: u16)->u16
    {
        debug.assert(1 <= month <= 12)
        if isLeapYear(year)
            return acast (DaysToMonth366[month] - DaysToMonth366[month - 1])
        return acast (DaysToMonth365[month] - DaysToMonth365[month - 1])
    }
}
