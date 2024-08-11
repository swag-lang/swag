#global testerror "Err0579"

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