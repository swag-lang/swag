//#global testerror "Err0030"

func(T) toto()
{
    var x: [..] T
}

#test
{
    toto'([2] s32)()
}