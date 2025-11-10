struct Err {}

func toto()->s32 throw
{
    throw Err{}
    return 666
}

func titi()->s32
{
    discard trycatch toto()
    @assert(false)
    return 666
}

#test
{
    let x = titi()
    @assert(x == 0)
}
