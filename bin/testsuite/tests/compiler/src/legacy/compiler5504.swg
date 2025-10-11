#global testerror "Err0574"

func toto()->{ x: s32 }
{
    return {0}
}

#test
{
    if toto() <= toto()
    {
    }
}
