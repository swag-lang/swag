#global testerror "Err0656"

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