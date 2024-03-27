func toto(x: s32)->s32
{
    //#print @nameof(toto)
    if x == 1 do return 1
    return x + #self(x - 1)
}

#test
{
    let x = toto(4)
    @assert(x == 10)
}