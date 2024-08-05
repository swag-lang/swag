#global testerror "Err0649"

func toto(x: string, y: s32) {}

#test
{
    toto(0, "a")
}