#global testerror "Err0598"

func toto(x: string, y: s32) {}

#test
{
    toto(0, "a")
}