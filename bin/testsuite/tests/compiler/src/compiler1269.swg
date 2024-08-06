#global testerror "Err0647"

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