#global testerror "Err1037"

func(T, N: u32) toto()
#validif
{
    #assert N > 0 and N <= 4, "N is out of range"
    return true
}
{
}

#test
{
    toto'(f32, 10)()
}