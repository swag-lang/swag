//#global testerror "Err0010"

func(T) toto()
{
    var x: [..] T
}

#test
{
    toto'([2] s32)()
}
