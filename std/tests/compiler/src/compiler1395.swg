struct DateTime
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
    func opEquals(self, other: Self)->bool
    {
        return @memcmp(self, other, @sizeof(Self)) == 0
    }
}

#test
{
    var x, y: DateTime
    @assert(x == y)
    x.millisecond = 1
    @assert(x != y)
}