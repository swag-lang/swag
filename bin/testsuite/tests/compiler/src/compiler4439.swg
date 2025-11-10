func toto(x: s32)
{
    @assert(x == 55)
}

func titi(x: &&s32)
{
    toto(x)
}

#test
{
    var x = 55
    titi(moveref &x)
}
