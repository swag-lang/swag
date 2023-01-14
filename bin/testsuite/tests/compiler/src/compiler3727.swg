//#global testerror
func toto()
{
    var x: any
    if x != null and x == 3
        @assert(false)
}

#test
{
    toto()
}