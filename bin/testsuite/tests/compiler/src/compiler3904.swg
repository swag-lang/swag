//#global testerror

func(T) toto(x: T)
{
    #assert @typeof(T) == const [2] s32
}

#test
{
    toto([1, 2])
}