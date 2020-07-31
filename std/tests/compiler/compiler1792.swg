enum A
{
    AA
    BB
}

#test
{
    using TOTO = A
    @assert(TOTO.AA == 0)
}