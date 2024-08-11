//#global testerror "Err0008"

func(T) toto()
{
    var x: [..] T
}

#test
{
    toto'([2] s32)()
}