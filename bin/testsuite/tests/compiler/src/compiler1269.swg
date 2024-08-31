#global testerror "Err0605"

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