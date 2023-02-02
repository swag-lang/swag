#global testerror

func toto() #selectif false
{
    @assert(false)
}

#test
{
    toto()
}