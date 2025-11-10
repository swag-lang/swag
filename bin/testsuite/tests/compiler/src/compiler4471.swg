func(T) toto(x: T)
{
    @assert(x % 3 == 2)
}

#test
{
    toto(5's8)
    toto(5'u8)
    toto(5's16)
    toto(5'u16)
}
