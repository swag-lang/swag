interface I
{
    func toto(self);
}

struct A
{
    xx: s32 = 666
}

var G: s32
impl I for A
{
    mtd impl toto()
    {
        G = xx
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    let itf: I = cast(I) x

    //var ptr: closure()
    let ptr = closure|itf|()
    {
        itf.toto()
    }

    ptr()
    @assert(G == 666)
}