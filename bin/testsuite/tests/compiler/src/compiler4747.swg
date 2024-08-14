#global testerror "Err0613"
struct(X: s32) TOTO
{
    v: [X] s32
}

func(X: s32) toto()->TOTO
{
    return {0}
}

#test
{
    discard toto'(4)()
}