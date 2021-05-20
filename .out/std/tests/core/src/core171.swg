
using Core.Time

#test
{
    {
        sp := TimeSpan.from(DateTime{2020, 11, 8})
        sp.addHours(1)
        @assert(sp.toDateTime().hour == 1)
        sp.addHours(24)
        @assert(sp.toDateTime().day == 9)
        sp.addHours(-24)
        @assert(sp.toDateTime().day == 8)
        sp.addMinutes(65)
        @assert(sp.toDateTime().hour == 2)
        @assert(sp.toDateTime().minute == 5)
    }

    {
        sp := TimeSpan.from(DateTime{2020, 11, 8})
        sp.addMonths(12)
        @assert(sp.toDateTime().year == 2021)
    }

    {
        sp := TimeSpan.from(DateTime{2020, 10, 31})
        sp.addMonths(1)
        @assert(sp.toDateTime().month == 11)
        @assert(sp.toDateTime().day == 30)
    }

    {
        sp := TimeSpan.from(DateTime{2020, 12, 31})
        sp.addDays(1)
        @assert(sp.toDateTime().year == 2021)
        @assert(sp.toDateTime().month == 1)
        @assert(sp.toDateTime().day == 1)
    }
}