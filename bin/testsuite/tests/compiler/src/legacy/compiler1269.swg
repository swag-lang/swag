#global testerror "Err0561"

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