#global testerror "Err0583"

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