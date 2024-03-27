//#global testerror

func(T) toto(x: T)
{
}

#test
{
    toto'(const [2] s32)([1, 2])
}
