#global testerror "Err0610"

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