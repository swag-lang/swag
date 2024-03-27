//#global testerror "Err0020"

func(T) toto()
{
    var x: [..] T
}

#test
{
    toto'([2] s32)()
}
