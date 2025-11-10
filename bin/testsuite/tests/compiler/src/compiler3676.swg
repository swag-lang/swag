interface I
{
    func toto(me);
}

struct A
{
    xx:  s32 = 666
}

var G: s32
impl I for A
{
    mtd impl toto()
    {
        G = me.xx
    }
}

//#[Swag.PrintBc]
#test
{
    var x:   A
    let itf: I = cast(I) x

    //var ptr: func||()
    let ptr = func|itf|()
    {
        itf.toto()
    }

    ptr()
    @assert(G == 666)
}
