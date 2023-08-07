#global skip
struct(X: s32) TOTO
{
    v: s32
}

func(X: s32) toto()->TOTO'X
{
    return {0}
}

#test
{
    discard toto'(0)()
}