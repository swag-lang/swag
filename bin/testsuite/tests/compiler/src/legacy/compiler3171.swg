func toto(x: s32)->s32
{
    return x
}

func titi()->s32
{
    return 1
}

#test
{
    discard toto(titi())
}
