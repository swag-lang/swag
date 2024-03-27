#global testerror "Err0178"

func toto(x: s32)
{
}

#test
{
    var y = &toto(1)
}
