#global testerror "Err0015"
func(T) toto(x: T)
{
    @assert(x == 0)
}

#test
{
    toto(0)
}