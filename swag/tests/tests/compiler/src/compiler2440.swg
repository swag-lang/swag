struct A
{
    v = 10
}

struct B
{
    v = 5
}

impl A
{
    func opCmp(using self, o: B)->s32
    {
        return v <=> o.v
    }
}

//#[swag.printbc]
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