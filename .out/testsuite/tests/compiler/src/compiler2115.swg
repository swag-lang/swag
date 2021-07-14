struct(T) Arr
{

}

interface IT
{
    toto: func(self)
    toto1: func(self)
}

var G: s32 = 0

impl IT for Arr
{
    //#[Swag.PrintBc]
    func toto(self)
    {
        G += 1
        @assert(@typeof(T) == s32)
    }

    func toto1(self)
    {
        G += 10
        @assert(@typeof(T) == f64)
    }
}

//#[Swag.PrintBc]
#test
{
    var x: Arr's32
    var t = cast(IT) x
    t.toto()
    @assert(G == 1)

    var x1: Arr'f64
    var t1 = cast(IT) x1
    t1.toto1()
    @assert(G == 11)
}