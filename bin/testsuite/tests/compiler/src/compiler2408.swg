#global testerror "Err0004"

func toto() #validif false
{
    @assert(false)
}

#test
{
    toto()
}