#global testerror "Err0080"

func toto() where false
{
    @assert(false)
}

#test
{
    toto()
}