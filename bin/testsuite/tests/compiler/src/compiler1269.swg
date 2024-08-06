#global testerror "Err0742"

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