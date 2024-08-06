#global testerror "Err0637"

func toto(fct: func(s32, s32)->bool)
{
}

#test
{
    toto(func(x, y, z)
    {
    })
}