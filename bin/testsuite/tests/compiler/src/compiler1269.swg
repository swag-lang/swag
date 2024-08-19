#global testerror "Err0591"

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