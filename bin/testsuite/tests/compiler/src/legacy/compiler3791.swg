//#global testerror "Err0244"
func(T) toto(x: T)
{
    @assert(x == 0)
}

#test
{
    toto(0)
}
