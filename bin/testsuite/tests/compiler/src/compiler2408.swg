#global testerror "Err0075"

func toto() where false
{
    @assert(false)
}

#test
{
    toto()
}