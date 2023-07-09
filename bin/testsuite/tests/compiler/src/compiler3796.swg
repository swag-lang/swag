var G = 0

func toto()->s32
{
    defer G = 1
    return 666
}

#test
{
    var x = toto()
    @assert(x == 666)
}