#global testerror "Err0591"

func(T) toto(x, y: T)
{
    #assert @typeof(T) == const [2] s32
}

#test
{
    toto([1, 2], [1, 2, 3])
}