#global testerror "Err0604"

enum A
{
    B
}

func toto(x: s32) {}

#test
{
    toto(A.B)
}
