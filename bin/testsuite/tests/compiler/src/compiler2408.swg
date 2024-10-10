#global testerror "Err0078"

func toto() where false
{
    @assert(false)
}

#test
{
    toto()
}