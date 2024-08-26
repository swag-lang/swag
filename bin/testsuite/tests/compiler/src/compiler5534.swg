//#global testerror "Err0308"

func toto(str: func(s32, s32))
{    
}

#test
{
    toto(func(x, y : s32) {})
}