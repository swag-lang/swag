//#global testerror "Err0194"
func(T) toto(x: T)
{
    @assert(x == 0)
}

#test
{
    toto(0)
}
