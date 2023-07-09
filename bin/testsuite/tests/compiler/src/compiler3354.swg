func toto(x: s32)->s32
{
    //#print @nameof(toto)
    if x == 1 return 1
    return x + #self(x - 1)
}

#test
{
    var x = toto(4)
    @assert(x == 10)
}