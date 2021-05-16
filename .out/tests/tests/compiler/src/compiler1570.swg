struct Point
{
    x: s64 = 1
}

impl Point
{
    func opDrop(using self)
    {

    }
}

func toto(val: *Point)
{
    @assert(val.x == 666)
    var y: Point =,move dref val
    @assert(y.x == 666)
    //@assert(val.x == 1)
}

#test
{
    var value: Point
    value.x = 666
    @assert(value.x == 666)
    toto(&value)
    //@assert(value.x == 1)
}
