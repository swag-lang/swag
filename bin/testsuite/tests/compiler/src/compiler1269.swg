#global testerror "Err0637"

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