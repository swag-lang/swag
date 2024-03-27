#global testerror "Err0649"

func(T) toto(x: T)
{
}

#test
{
    toto'([2] s32)([1, 2])
}
