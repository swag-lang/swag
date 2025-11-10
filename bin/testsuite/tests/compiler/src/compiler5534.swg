//#global testerror "Err0468"

func toto(str: func(s32, s32)) {}

#test
{
    toto(func(x, y: s32) {})
}
