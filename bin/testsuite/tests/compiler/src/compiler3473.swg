#[Swag.Inline]
func toto(x: func(...))
{
    x(40, 50, 60, 70)
}

#test
{
    toto(func(x: ...)
    {
        @assert(@countof(x) == 4)
        @assert(cast(s32) x[0] == 40)
        @assert(cast(s32) x[1] == 50)
        @assert(cast(s32) x[2] == 60)
        @assert(cast(s32) x[3] == 70)
    })
}
