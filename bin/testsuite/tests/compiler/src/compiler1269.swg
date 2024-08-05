#global testerror "Err0649"

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