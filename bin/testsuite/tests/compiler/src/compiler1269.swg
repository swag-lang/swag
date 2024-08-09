#global testerror "Err0622"

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