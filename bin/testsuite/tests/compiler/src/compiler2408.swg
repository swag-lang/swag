#global testerror "Err0076"

func toto() where false
{
    @assert(false)
}

#test
{
    toto()
}