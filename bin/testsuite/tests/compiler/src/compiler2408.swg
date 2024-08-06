#global testerror "Err0077"

func toto() where false
{
    @assert(false)
}

#test
{
    toto()
}