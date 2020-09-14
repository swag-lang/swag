func(T) toto(x: T)
{
    #assert @typeof(T) == bool
}

func(T) titi(x: T)
{
    #assert @typeof(T) == s32
    toto(true)
}

#test
{
    titi's32(0)
}