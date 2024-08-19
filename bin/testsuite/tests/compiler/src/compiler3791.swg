//#global testerror "Err0538"
func(T) toto(x: T)
{
    @assert(x == 0)
}

#test
{
    toto(0)
}