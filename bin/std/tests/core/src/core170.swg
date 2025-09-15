using Core.Time

#test
{
    {
        let x: DateTime{2020, 11, 8}
        @assert(x.dayOfWeek() == DayOfWeek.Sunday)
    }
    {
        let x: DateTime{2019, 7, 3}
        @assert(x.dayOfWeek() == DayOfWeek.Wednesday)
    }
    {
        let x: DateTime{1971, 9, 5}
        @assert(x.dayOfWeek() == DayOfWeek.Sunday)
    }
}
