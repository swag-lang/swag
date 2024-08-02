#global testerror "Err0644"

func(T) toto(x, y: T)
{
    #assert @typeof(T) == const [2] s32
}

#test
{
    toto([1, 2], [1, 2, 3])
}