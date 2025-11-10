func toto(x: s32...)
{
    @assert(@countof(x) == 2)
    @assert(cast(s32) x[0] == 4)
    @assert(cast(s32) x[1] == 5)
}

//#[Swag.PrintBc]
#test
{
    let x: func(s32...) = &toto
    x(4, 5)
}
