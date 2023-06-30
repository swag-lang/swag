//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var xx: s64 = 42

    var x: closure(s32, ...) = closure|xx|(x: s32, y: ...)
    {
        //@print(xx)
        @assert(x == 52)
        @assert(xx == 42)
    }

    var xx1: s32 = 52
    xx1.x(4)
}