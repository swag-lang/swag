//#[Swag.PrintBc]
#test
{
    let x: func||(...) = func||(x: ...)
    {
        @assert(@countof(x) == 2)
        @assert(cast(s32) x[0] == 4)
        @assert(cast(s32) x[1] == 5)
    }

    x(4, 5)
}
