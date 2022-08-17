#global testerror
func(T) toto(x: T)
{
    @assert(x == 0)
}

#test
{
    toto(0)
}