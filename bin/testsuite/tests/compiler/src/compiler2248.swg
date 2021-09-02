struct A
{
    lambda: func(s32)
}

func titi(p: s32)
{
}

#test
{
    var res: A
    res.lambda = &titi
    res.lambda(0)
}