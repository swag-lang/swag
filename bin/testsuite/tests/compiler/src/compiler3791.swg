//#global testerror "Err0012"
func(T) toto(x: T)
{
    @assert(x == 0)
}

#test
{
    toto(0)
}