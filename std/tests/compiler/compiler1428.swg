/*
using swag

struct A
{
    x: s64 = 10
}

var G: s32

impl A
{
    func opPostFromMove(using self)
    {
        G += 1
        @assert(x == 10)
    }
}

#test
{
    G = 0
    
    var a: A
    var b: A
    a = move b
    @assert(G == 1)
	@assert(b.x == 0)
}*/