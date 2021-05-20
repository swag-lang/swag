struct A
{
    v = 555
}

impl A
{
    func opEquals(using self, o: s32)->bool
    {
        return o == 666
    }

    func opCmp(using self, o: s32)->s32
    {
        return v <=> o
    }
}

//#[Swag.printbc]
#test
{
    var x: A
    @assert(666 == x)
    @assert(x == 666)

    @assert(666 <=> x == 1)
    @assert(x <=> 666 == -1)

    @assert(666 > x)
    @assert(x < 666)

    @assert(666 >= x)
    @assert(x <= 666)
    @assert(555 >= x)
    @assert(x <= 555)

    @assert(444 < x)
    @assert(x > 444)

    @assert(444 <= x)
    @assert(x >= 444)
    @assert(555 <= x)
    @assert(x >= 555)
}