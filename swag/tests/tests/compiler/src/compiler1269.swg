
#global testerror

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