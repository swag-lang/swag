using Swag

struct A
{
    x: s64 = 10
}

var G: s32

impl A
{
    func opPostMove(using self)
    {
        G += 1
        @assert(x == 20)
    }
}

#test
{
    G = 0

    var a: A
    var b: A
    b.x = 20
    a =,move b
    @assert(G == 1)
    //@assert(b.x == 10)
}