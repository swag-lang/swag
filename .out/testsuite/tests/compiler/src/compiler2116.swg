struct(T) Arr
{
    enum BB: T
    {
        R
        G
        B
    }

    /*struct ST
    {
        x, y: T
    }*/
}

//#[Swag.PrintBc]
#test
{
    var x: Arr's32
    ptr := @typeof(x.BB)
    @assert(ptr.rawType == s32)
    @assert(x.BB.R == 0)
    @assert(x.BB.G == 1)
    @assert(x.BB.B == 2)
}