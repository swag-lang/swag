func toto(v: s64...)
{
    @assert(@countof(v) == 4)
    @assert(v[0] == 1)
    @assert(v[1] == 2)
    @assert(v[2] == 3)
    @assert(v[3] == 4)
}

//#[swag.printbc]
#test
{
    x := @[1's64, 2, 3, 4]
    toto(@spread(x))
}