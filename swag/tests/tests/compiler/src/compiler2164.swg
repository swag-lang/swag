struct A
{
}

var G1: s32 = 0

func close(p: *A)
{
    G1 += 1
}

struct B
{
    x: A
}

var G: s32 = 0

impl B
{
    func close(using self)
    {
        G += 1
        @assert(G == 1)
        x.close()
    }
}

#test
{
    var v: B
    v.close()
    @assert(G1 == 1)
}