#global public
#global namespace Time

const TicksPerMilliSecond: u64 = 10000
const TicksPerSecond:      u64 = TicksPerMilliSecond * 1000
const TicksPerMinute:      u64 = TicksPerSecond * 60
const TicksPerHour:        u64 = TicksPerMinute * 60
const TicksPerDay:         u64 = TicksPerHour * 24
const MillisPerSecond      = 1000'u64
const MillisPerMinute      = MillisPerSecond * 60
const MillisPerHour        = MillisPerMinute * 60
const MillisPerDay         = MillisPerHour * 24
const DaysPerYear          = 365
const DaysPer4Years        = (DaysPerYear * 4) + 1
const DaysPer100Years      = (DaysPer4Years * 25) - 1
const DaysPer400Years      = (DaysPer100Years * 4) + 1
const DaysToMonth365       = [0'u16, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365]
const DaysToMonth366       = [0'u16, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366]
const MaxMilliSeconds      = Swag.U64.Max / TicksPerMilliSecond

const DaysTo1601:  u64 = (DaysPer400Years * 4)                                                                  // Number of days from 1/1/0001 to 12/31/1600
const DaysTo1899:  u64 = ((DaysPer400Years * 4) + (DaysPer100Years * 3)) - 367                                  // Number of days from 1/1/0001 to 12/30/1899
const DaysTo1970:  u64 = (DaysPer400Years * 4) + (DaysPer100Years * 3) + (DaysPer4Years * 17) + DaysPerYear     // Number of days from 1/1/0001 to 12/31/1969
const DaysTo10000: u64 = (DaysPer400Years * 25) - 366                                                           // Number of days from 1/1/0001 to 12/31/9999
const MaxTicks:    u64 = (DaysTo10000 * TicksPerDay) - 1
const MaxMillis:   u64 = (DaysTo10000 * MillisPerDay)

// Returns the current time expressed in milliseconds
func nowMilliseconds()->u32
{
    let ticks = nowPrecise() / (preciseFrequency() / 1000)
    return cast<bit>(u32) ticks
}

// Returns the current time expressed in microseconds
func nowMicroseconds()->u32
{
    let ticks = nowPrecise() / (preciseFrequency() / 1000000)
    return cast<bit>(u32) ticks
}

alias Ticks = u64

// Convert a time to a 64 bits value
func timeToTicks(hour, minute, second, millisecond: u16)->Ticks
{
    Debug.assert(0 <= hour and hour < 24)
    Debug.assert(0 <= minute and minute < 60)
    Debug.assert(0 <= second and second < 60)
    Debug.assert(0 <= millisecond and millisecond < 1000)

    // totalSeconds is bounded by 2^31 * 2^12 + 2^31 * 2^8 + 2^31,
    // which is less than 2^44, meaning we won't overflow totalSeconds.
    var totalMilli = (hour * 3600'u32 + minute * 60'u32 + second) * 1000'u64
    totalMilli += millisecond
    Debug.assert(totalMilli <= MaxMilliSeconds)
    return totalMilli * TicksPerMilliSecond
}

// Convert a 64 bits tick value to a time
func ticksToTime(ticks: Ticks)->{ hour: u16, minute: u16, second: u16, millisecond: u16 }
{
    var result: retval = undefined
    result.hour        = cast() ((ticks / TicksPerHour) % 24)
    result.minute      = cast() ((ticks / TicksPerMinute) % 60)
    result.second      = cast() ((ticks / TicksPerSecond) % 60)
    result.millisecond = cast() ((ticks / TicksPerMilliSecond) % 1000)
    return result
}

// Convert a date to a 64 bits value
func dateToTicks(year, month, day: u16)->Ticks
{
    Debug.assert(1 <= year and year <= 9999)
    Debug.assert(1 <= month and month <= 12)
    Debug.assert(1 <= day and day <= daysInMonth(year, month))

    let days = isLeapYear(year) ? @dataof(DaysToMonth366) : @dataof(DaysToMonth365)
    let y    = cast(u64) (year - 1)
    let n    = y * 365 + #wrap y / 4 + #wrap y / 400 + #wrap days[month - 1] + #wrap day - 1 - #wrap y / 100
    return n * TicksPerDay
}

// Convert a 64 bits value to a date
func ticksToDate(ticks: Ticks)->{ year: u16, month: u16, day: u16 }
{
    // n = number of days since 1/1/0001
    var n = ticks / TicksPerDay
    // y400 = number of whole 400-year periods since 1/1/0001
    let y400 = n / DaysPer400Years
    // n = day number within 400-year period
    n -= y400 * DaysPer400Years
    // y100 = number of whole 100-year periods within 400-year period
    var y100 = n / DaysPer100Years
    // Last 100-year period has an extra day, so decrement result if 4
    if y100 == 4:
        y100 = 3
    // n = day number within 100-year period
    n -= y100 * DaysPer100Years
    // y4 = number of whole 4-year periods within 100-year period
    let y4 = n / DaysPer4Years
    // n = day number within 4-year period
    n -= y4 * DaysPer4Years
    // y1 = number of whole years within 4-year period
    var y1 = n / DaysPerYear
    // Last year has an extra day, so decrement result if 4
    if y1 == 4:
        y1 = 3
    // n = day number within year
    n -= y1 * DaysPerYear
    // dayOfYear = n + 1;
    // Leap year calculation looks different from IsLeapYear since y1, y4,
    // and y100 are relative to year 1, not year 0
    let leapYear = y1 == 3 and (y4 != 24 or y100 == 3)
    let days     = leapYear ? @dataof(DaysToMonth366) : @dataof(DaysToMonth365)
    // All months have less than 32 days, so n >> 5 is a good conservative
    // estimate for the month
    var m = (n >> 5) + 1
    // m = 1-based month number
    while n >= days[m]:
        m += 1

    var result: retval = undefined
    result.year  = cast() (y400 * 400 + y100 * 100 + y4 * 4 + y1 + 1)
    result.month = cast() m
    result.day   = cast() (n - days[m - 1] + 1)
    return result
}

// Returns true if the given year is a leap year
func isLeapYear(year: u16)->bool
{
    return year % 4 == 0 and (year % 100 != 0 or year % 400 == 0)
}

// Returns the number of days of the given month, for the given year
func daysInMonth(year, month: u16)->u16
{
    Debug.assert(1 <= month and month <= 12)
    if isLeapYear(year):
        return cast() (DaysToMonth366[month] - DaysToMonth366[month - 1])
    return cast() (DaysToMonth365[month] - DaysToMonth365[month - 1])
}
