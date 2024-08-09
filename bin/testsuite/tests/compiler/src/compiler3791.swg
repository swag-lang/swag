//#global testerror "Err0023"
func(T) toto(x: T)
{
    @assert(x == 0)
}

#test
{
    toto(0)
}