//#global testerror "Err0023"

func(T) toto()
{
    var x: [..] T
}

#test
{
    toto'([2] s32)()
}