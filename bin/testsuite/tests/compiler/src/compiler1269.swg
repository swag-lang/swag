#global testerror "Err0652"

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