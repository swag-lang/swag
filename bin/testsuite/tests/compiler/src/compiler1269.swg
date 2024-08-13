#global testerror "Err0582"

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