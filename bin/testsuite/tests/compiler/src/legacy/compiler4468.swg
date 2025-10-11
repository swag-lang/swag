func(T) toto(x: T)
{
    @assert(x * 3 == 6)
}

#test
{
    toto(2's8)
    toto(2'u8)
    toto(2's16)
    toto(2'u16)
}
