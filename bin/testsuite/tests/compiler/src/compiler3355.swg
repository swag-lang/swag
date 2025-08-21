func toto(x: s32)
{
    let y = #me.x
    @assert(y == 10)
}

#test
{
    toto(10)
}