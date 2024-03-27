enum A
{
    AA
    BB
}

#test
{
    typealias TOTO = A
    @assert(TOTO.AA == 0)
}
