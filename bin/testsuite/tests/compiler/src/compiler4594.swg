//#[Swag.PrintBc]
#test
{
    var xx: s32 = 42

    var x: closure(s32, ...) = closure|xx|(x: s32, y: ...)
    {
        //@assert(xx == 42)
        @assert(x == 52)
        @assert(cast(s32) y[0] == 4)
    }

    var xx1: s32 = 52
    xx1.x(4)
}