interface I
{
    mtd toto(x: s32 = 10);
}

struct A {}

var G = 0
impl I for A
{
    mtd impl toto(x: s32 = 20)
    {
        G += x
    }
}

#test
{
    var a: A
    let i  = cast(I) a
    i.toto()
    a.I.toto()
    @assert(G == 30)
}
