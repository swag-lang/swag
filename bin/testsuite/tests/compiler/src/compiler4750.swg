//#global skip
func(X: s32) toto()->{v: s32}
{
    return {0}
}

#test
{
    let s  = toto'(4)()
    let s1 = toto'(5)()
}
