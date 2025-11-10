//#[Swag.PrintBc]
#test
{
    let xx: s32 = 42

    let x: func||(s32, ...) = func|xx|(x: s32, y: ...)
    {
        //@assert(xx == 42)
        @assert(x == 52)
        @assert(cast(s32) y[0] == 4)
    }

    let xx1: s32 = 52
    xx1.x(4)
}
