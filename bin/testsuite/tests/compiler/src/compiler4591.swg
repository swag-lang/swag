//#[Swag.PrintBc]
#test
{
    let x: func||(s32) = func(x: s32)
    {
        //@print(x)
        @assert(x == 52)
    }

    let xx: s32 = 52
    xx.x()
}