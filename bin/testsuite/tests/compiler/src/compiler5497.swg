#global testerror "Err0440"

func toto32(s: string, x: s32)->s32
{
    return 0
}

#test
{
    discard toto32("titi", x: cast(s32) @spread([1, 2, 4]))
}