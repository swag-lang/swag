func toto()->{ a, b: u32 }
{
    return {5, 10}
}

struct A { x, y: u32 }

#test
{
    var v: A
    (v.x, v.y) = toto()
    @assert(v.x == 5)
    @assert(v.y == 10)
    //var (x1, y1) = toto()
}
