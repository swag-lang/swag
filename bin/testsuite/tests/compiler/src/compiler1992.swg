struct A
{
    x: s32
}

var G = 0
impl A
{
    func opCmp(using me, other: A)->s32
    {
        G += 1
        return x <=> other.x
    }
}

#test
{
    var x = A{10}
    var y = A{20}
    @assert((x <=> y) == -1)
    x.x = 30
    @assert((x <=> y) == 1)
    x.x = 20
    @assert((x <=> y) == 0)

    @assert(G == 3)
}