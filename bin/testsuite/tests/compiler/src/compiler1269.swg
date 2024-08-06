#global testerror "Err0609"

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