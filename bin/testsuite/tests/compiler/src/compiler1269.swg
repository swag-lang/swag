#global testerror "Err0549"

enum A
{
    B
}

func toto(x: s32)
{
}

#test
{
    toto(A.B)
}