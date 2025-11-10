struct AA {}

impl AA
{
    #[Swag.Implicit]
    func opCast(me)->s32
    {
        return 0
    }
}

var G = 0

//#[Swag.Inline]
func(T) toto(x: T)
{
    if T == s32 do
        G += 1
    if T == AA do
        G += 2
}

//#[Swag.PrintBc]
#test
{
    var x: AA
    toto(0's32)
    toto(x)
    toto(x)

    @assert(G == 5)
}
