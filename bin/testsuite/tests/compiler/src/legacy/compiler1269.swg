#global testerror "Err0745"

enum A
{
    B
}

func toto(x: s32) {}

#test
{
    toto(A.B)
}
