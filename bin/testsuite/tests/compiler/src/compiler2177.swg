const TicksPerMillisecond: u64 = 10000
const TicksPerSecond:      u64 = TicksPerMillisecond * 1000
const TicksPerMinute:      u64 = TicksPerSecond * 60
const TicksPerHour:        u64 = TicksPerMinute * 60
const TicksPerDay:         u64 = TicksPerHour * 24

const DaysPerYear     = 365
const DaysPer4Years   = (DaysPerYear * 4) + 1
const DaysPer100Years = (DaysPer4Years * 25) - 1
const DaysPer400Years = (DaysPer100Years * 4) + 1

const DaysToMonth365 = [0'u16, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365]
const DaysToMonth366 = [0'u16, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366]

func dateToTicks(year, month, day: u16)->u64
{
    let days = @dataof(DaysToMonth366)
    let y    = cast(u64) (year - 1)
    let n    = ((y * 365) + (y / 4) + (y / 400) + days[month - 1] + (day - 1)) - (y / 100)
    return n * TicksPerDay
}

#test
{
    let ticks = dateToTicks(2020, 11, 6)
    @assert(ticks == 637402176000000000)
}
