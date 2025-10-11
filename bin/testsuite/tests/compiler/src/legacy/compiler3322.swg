//#global skip
struct AA {}

interface BB
{
    func x(me)->s32;
}

var G = 0

impl BB for AA
{
    mtd impl x()->s32
    {
        G = 1
        return 555
    }
}

//#[Swag.PrintBc]
#test
{
    var x:  AA
    let ptr = &x
    let itf = cast(BB) ptr
    @assert(itf.x() == 555)
    @assert(G == 1)
}
