#global testerror "Err0594"

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