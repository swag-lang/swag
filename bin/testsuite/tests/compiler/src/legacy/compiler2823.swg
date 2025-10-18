#global testerror "Err0641"

func toto(x: s32) {}

#test
{
    const aaa = 0
    aaa.toto(0)
}
