#global testerror

func toto(fct: func(s32, s32)->bool)
{
}

#test
{
    toto(@(x, y)
    {
    })
}