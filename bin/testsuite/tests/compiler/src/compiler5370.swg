//#global skip
var G = 0

struct B
{
    b:   s32
}

impl B
{
    mtd opEquals(o: B)->bool
    {
        G += 1
        return me.b == o.b
    }
}

#test
{
    let a = {0, 1, B{2}}
    var b = {0, 1, B{3}}
    @assert(a != b)
    @assert(G == 1)
    b = {0, 1, B{2}}
    @assert(a == b)
    @assert(G == 2)
}
