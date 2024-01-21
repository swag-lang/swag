#global testerror "Err0652"

func toto(fct: func(s32, s32)->bool)
{
}

#test
{
    toto(func(x, y, z)
    {
    })
}