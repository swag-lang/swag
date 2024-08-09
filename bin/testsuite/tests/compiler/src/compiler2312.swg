#global testerror "Err0622"

func toto32(s: string, x: s32)->s32
{
    return 0
}

#test
{
    toto32("titi", x: @spread([1, 2, 4]))
}