//#[Swag.PrintBc]
#test
{
    let x: closure(s32...) = closure||(x: s32...)
    {
        @assert(@countof(x) == 2)
        @assert(cast(s32) x[0] == 4)
        @assert(cast(s32) x[1] == 5)
    }

    x(4, 5)
}
