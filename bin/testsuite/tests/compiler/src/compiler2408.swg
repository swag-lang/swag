#global testerror "Err0083"

func toto() where false
{
    @assert(false)
}

#test
{
    toto()
}