//#global testerror
func(A) toto(x: {a:A})
{
    #assert A == #type [2] s32
    @assert(x.item0[0] == 1)
    @assert(x.item0[1] == 2)
}

#test
{
    toto({[1, 2]})
}