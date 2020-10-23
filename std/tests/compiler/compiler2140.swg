func toto(v: string...)
{
    @assert(@countof(v) == 4)
    @assert(v[0] == "AA")
    @assert(v[1] == "BBB")
    @assert(v[2] == "CCCC")
    @assert(v[3] == "DDDDD")
}

//#[swag.printbc]
#test
{
    x := @["AA", "BBB", "CCCC", "DDDDD"]
    toto(@spread(x))
}