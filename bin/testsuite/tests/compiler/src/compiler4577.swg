func(T) toto()
{
    var x: [2] T
    @assert(@countof(x) == 2)
}

#test
{
    toto'([3] s32)()
}
