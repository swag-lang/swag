#global testerror "Err0423"

func(T, N: u32) toto()
where
{
    #assert(N > 0 and N <= 4, "tot")
    return true
}
{
}

#test
{
    toto'(f32, 10)()
}