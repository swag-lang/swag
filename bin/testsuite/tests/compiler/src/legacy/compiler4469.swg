func(T) toto(x: T)
{
    @assert(x / 3 == 2)
}

#test
{
    toto(6's8)
    toto(6'u8)
    toto(6's16)
    toto(6'u16)
}
