#global testerror "Err0188"

func toto()
    where false
{
    @assert(false)
}

#test
{
    toto()
}
