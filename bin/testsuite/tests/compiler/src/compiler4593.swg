//#[Swag.PrintBc]
#test
{
    var x: closure(s32, ...) = func(x: s32, y: ...)
    {
        //@print(x)
        @assert(x == 52)
    }

    var xx: s32 = 52
    xx.x()
}