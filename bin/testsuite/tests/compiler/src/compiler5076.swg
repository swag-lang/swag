//#global testerror "Err0022"

func(T) toto()
{
    var x: [..] T
}

#test
{
    toto'([2] s32)()
}