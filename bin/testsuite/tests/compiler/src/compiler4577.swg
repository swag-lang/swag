func(T) toto()
{
    var x: [2] T
    @assert(@countof(x) == 2)
}

#test
{
    toto'(#type [3] s32)()
}
