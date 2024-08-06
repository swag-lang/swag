#global testerror "Err0602"

func toto(x: string, y: s32) {}

#test
{
    toto(0, "a")
}