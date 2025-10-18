#global testerror "Err0403"

func toto()
    where false
{
    @assert(false)
}

#test
{
    toto()
}
