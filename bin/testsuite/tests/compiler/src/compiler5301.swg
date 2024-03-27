func toto(x: const &s32)->s32
{
    let arr = [1, 52, 10]
    return arr[x]
}

#test
{
    @assert(toto(0) == 1)
    @assert(toto(1) == 52)
    @assert(toto(2) == 10)
}
