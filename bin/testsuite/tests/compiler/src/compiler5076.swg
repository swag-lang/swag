//#global testerror "Err0010"

func(T) toto()
{
    var x: [..] T
}

#test
{
    toto'(#type [2] s32)()
}
