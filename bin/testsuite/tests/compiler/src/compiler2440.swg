struct A
{
    v    = 10
}

struct B
{
    v    = 5
}

impl A
{
    func opCmp(me, o: B)->s32
    {
        return me.v <=> o.v
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    var y: B
    @assert(x <=> y == 1)
    @assert(y <=> x == -1)
    @assert(y < x)
    y.v = 20
    @assert(y > x)
}
