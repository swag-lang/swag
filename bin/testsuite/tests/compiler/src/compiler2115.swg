struct(T) Arr
{

}

interface IT
{
    func toto(self);
    func toto1(self);
}

var G: s32 = 0

impl IT for Arr
{
    //#[Swag.PrintBc]
    func impl toto(self)
    {
        G += 1
        @assert(#typeof(T) == s32)
    }

    func impl toto1(self)
    {
        G += 10
        @assert(#typeof(T) == f64)
    }
}

//#[Swag.PrintBc]
#test
{
    var x: Arr's32
    let t = cast(IT) x
    t.toto()
    @assert(G == 1)

    var x1: Arr'f64
    let t1 = cast(IT) x1
    t1.toto1()
    @assert(G == 11)
}