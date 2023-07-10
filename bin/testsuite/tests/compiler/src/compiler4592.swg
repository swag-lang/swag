//#[Swag.PrintBc]
#test
{
    let x: closure(s32, ...) = closure||(x: s32, y: ...)
    {
        //@print(x)
        @assert(x == 52)
    }

    var xx: s32 = 52
    xx.x()
}