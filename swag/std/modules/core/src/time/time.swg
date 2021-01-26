#global public
#global namespace time

protected
{
    const TicksPerMilliSecond: u64 = 10000
    const TicksPerSecond: u64 = TicksPerMilliSecond * 1000
    const TicksPerMinute: u64 = TicksPerSecond * 60
    const TicksPerHour: u64   = TicksPerMinute * 60
    const TicksPerDay: u64    = TicksPerHour * 24
    const MillisPerSecond = 1000'u64
    const MillisPerMinute = MillisPerSecond * 60
    const MillisPerHour   = MillisPerMinute * 60
    const MillisPerDay    = MillisPerHour * 24
    const DaysPerYear     = 365
    const DaysPer4Years   = (DaysPerYear * 4) + 1
    const DaysPer100Years = (DaysPer4Years * 25) - 1
    const DaysPer400Years = (DaysPer100Years * 4) + 1
    const DaysToMonth365  = @[0'u16, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365]
    const DaysToMonth366  = @[0'u16, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366]
    const MaxMilliSeconds = swag.MaxU64 / TicksPerMilliSecond

    const DaysTo1601: u64  = (DaysPer400Years * 4) // Number of days from 1/1/0001 to 12/31/1600
    const DaysTo1899: u64  = ((DaysPer400Years * 4) + (DaysPer100Years * 3)) - 367 // Number of days from 1/1/0001 to 12/30/1899
    const DaysTo1970: u64  = (DaysPer400Years * 4) + (DaysPer100Years * 3) + (DaysPer4Years * 17) + DaysPerYear // Number of days from 1/1/0001 to 12/31/1969
    const DaysTo10000: u64 = (DaysPer400Years * 25) - 366  // Number of days from 1/1/0001 to 12/31/9999
    const MaxTicks: u64    = (DaysTo10000 * TicksPerDay) - 1
    const MaxMillis: u64   = (DaysTo10000 * MillisPerDay)
}

// Returns the current time expressed in milliseconds
func nowMilliseconds()->u32
{
    ticks := nowPrecise() * 1000
    ticks /= preciseFrequency()
    return autocast ticks
}

// Returns the current time expressed in microseconds
func nowMicroseconds()->u32
{
    ticks := nowPrecise() * 1000000
    ticks /= preciseFrequency()
    return autocast ticks
}

alias Ticks = u64

// Convert a time to a 64 bits value
func timeToTicks(hour, minute, second, millisecond: u16)->Ticks
{
    debug.assert(0 <= hour && hour < 24)
    debug.assert(0 <= minute && minute < 60)
    debug.assert(0 <= second && second < 60)
    debug.assert(0 <= millisecond && millisecond < 1000)

    // totalSeconds is bounded by 2^31 * 2^12 + 2^31 * 2^8 + 2^31,
    // which is less than 2^44, meaning we won't overflow totalSeconds.
    totalMilli := (hour*3600 + minute*60 + second) * 1000'u64
    totalMilli += millisecond
    debug.assert(totalMilli <= MaxMilliSeconds)
    return totalMilli * TicksPerMilliSecond
}

// Convert a 64 bits tick value to a time
func ticksToTime(ticks: Ticks) -> {hour: u16, minute: u16, second: u16, millisecond: u16}
{
    var result: retval = ?
    result.hour = autocast ((ticks / TicksPerHour) % 24)
    result.minute = autocast ((ticks / TicksPerMinute) % 60)
    result.second = autocast ((ticks / TicksPerSecond) % 60)
    result.millisecond = autocast ((ticks / TicksPerMilliSecond) % 1000)
    return result
}

// Convert a date to a 64 bits value
func dateToTicks(year, month, day: u16)->Ticks
{
    debug.assert(1 <= year && year <= 9999)
    debug.assert(1 <= month && month <= 12)
    debug.assert(1 <= day && day <= daysInMonth(year, month))

    days := isLeapYear(year) ? @dataof(DaysToMonth366) : @dataof(DaysToMonth365)
    y := cast(u64) (year - 1)
    #[swag.safety("of", false)]
    n := y*365 + y/4 + y/400 + days[month-1] + day-1 - y/100
    return n * TicksPerDay
}

// Convert a 64 bits value to a date
func ticksToDate(ticks: Ticks) -> {year: u16, month: u16, day: u16}
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
    while n >= days[m] m += 1

    var result: retval = ?
    result.year  = autocast (y400*400 + y100*100 + y4*4 + y1 + 1)
    result.month = autocast (m)
    result.day   = autocast (n - days[m-1] + 1)
    return result
}

// Returns true if the given year is a leap year
func isLeapYear(year: u16)->bool
{
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)
}

// Returns the number of days of the given month, for the given year
func daysInMonth(year, month: u16)->u16
{
    debug.assert(1 <= month && month <= 12)
    if isLeapYear(year)
        return autocast (DaysToMonth366[month] - DaysToMonth366[month - 1])
    return autocast (DaysToMonth365[month] - DaysToMonth365[month - 1])
}
