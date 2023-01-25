func(T, N: u32) toto()
#selectifonce
{
    #assert(N > 0 and N <= 4)
    return true
}
{
}

#test
{
    toto'(f32, 4)()
}