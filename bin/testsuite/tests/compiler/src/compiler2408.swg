#global testerror "Err0060"

func toto() where false
{
    @assert(false)
}

#test
{
    toto()
}