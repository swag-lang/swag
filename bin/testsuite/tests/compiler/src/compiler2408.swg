#global testerror "Err0062"

func toto() where false
{
    @assert(false)
}

#test
{
    toto()
}