func(T, N: u32) toto()
#selectif
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