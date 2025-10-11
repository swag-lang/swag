//#global testerror

func toto(x: s32)
{
    @assert(x == 4)
}

#test
{
    var v: func||(s32)
    v = &toto
    v(4)
}
