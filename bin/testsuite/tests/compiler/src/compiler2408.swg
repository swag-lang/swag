#global testerror

func toto() #validif false
{
    @assert(false)
}

#test
{
    toto()
}