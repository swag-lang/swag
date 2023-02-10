func toto(x: s32)
{
    @assert(x == 55)
}

func titi(x: moveref s32)
{
    toto(x)
}

#test
{
    x := 55
    titi(moveref &x)
}