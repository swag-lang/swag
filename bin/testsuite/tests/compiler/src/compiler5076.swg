//#global testerror "Err0182"

func(T) toto()
{
    var x: [..] T
}

#test
{
    toto'([2] s32)()
}