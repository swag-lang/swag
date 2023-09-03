#global testerror "Err0053"

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