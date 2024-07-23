#global testerror "Err0085"

func toto() where false
{
    @assert(false)
}

#test
{
    toto()
}