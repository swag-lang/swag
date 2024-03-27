func tata()->s32
{
    return 555
}

func titi()->s32
{
    return tata()
}

func toto()->s32
{
    defer
    {
        discard titi()
    }

    return 666
}

#test
{
    let x = toto()
    @assert(x == 666)
}
