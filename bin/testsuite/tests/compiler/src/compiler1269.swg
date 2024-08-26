#global testerror "Err0598"

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