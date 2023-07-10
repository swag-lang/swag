func toto()->s32 throw
{
    throw "err"
    return 666
}

func titi()->s32
{
    discard trycatch toto()
    return 666
}

func tata()
{
    trycatch
    {
        let x = titi()
        @assert(x == 0)
    }
}

#test
{
    tata()
}