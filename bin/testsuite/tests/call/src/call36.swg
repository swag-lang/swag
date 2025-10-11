using test_foreign

//#[Swag.PrintBc]
#test
{
    toto36(func(x: ...)
    {
        //@print(@countof(x))
        @assert(@countof(x) == 4)
        @assert(cast(s32) x[0] == 40)
        @assert(cast(s32) x[1] == 50)
        @assert(cast(s32) x[2] == 60)
        @assert(cast(s32) x[3] == 70)
    })
}
