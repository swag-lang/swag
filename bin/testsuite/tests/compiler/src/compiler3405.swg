#global testerror

func toto(x: s32) {}

#test
{
    var v: closure(s32)
    v = &toto
}