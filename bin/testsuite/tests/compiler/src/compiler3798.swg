func titi()->s32
{
    return 555
}

func toto()->s32
{
    defer
    {
        y := titi()
        @assert(y == 555)
    }

    return 666
}

#test
{
    x := toto()
    @assert(x == 666)
}