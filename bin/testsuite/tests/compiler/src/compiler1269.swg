#global testerror "Err0608"

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