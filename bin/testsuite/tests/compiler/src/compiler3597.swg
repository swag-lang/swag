#global testerror "Err0116"

func toto(x: s32) {}
func toto(x: s8) {}

#test
{
    toto(0)
}