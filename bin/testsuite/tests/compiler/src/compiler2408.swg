#global testerror

func toto() #selectifonce false
{
    @assert(false)
}

#test
{
    toto()
}