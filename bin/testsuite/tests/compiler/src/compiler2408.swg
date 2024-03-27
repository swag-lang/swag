#global testerror "Err0085"

func toto()
    #validif false
{
    @assert(false)
}

#test
{
    toto()
}
