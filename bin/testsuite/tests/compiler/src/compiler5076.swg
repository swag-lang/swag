//#global testerror "Err0031"

func(T) toto()
{
    var x: [..] T
}

#test
{
    toto'([2] s32)()
}