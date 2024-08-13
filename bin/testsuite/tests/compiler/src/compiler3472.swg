#global testerror "Err0582"

//#[Swag.PrintBc]
#test
{
    var x: closure(bool, s32...) = closure||(b: bool, x: s32...)
    {
        @assert(@countof(x) == 2)
        @assert(cast(s32) x[0] == 4)
        @assert(cast(s32) x[1] == 5)
    }

    x(4, 5)
}