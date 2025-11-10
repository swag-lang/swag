//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let xx: s64 = 42

    let x: func||(s32, ...) = func|xx|(x: s32, y: ...)
    {
        //@print(xx)
        @assert(x == 52)
        @assert(xx == 42)
    }

    let xx1: s32 = 52
    xx1.x(4)
}
