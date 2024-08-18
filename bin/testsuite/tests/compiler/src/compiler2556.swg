#global testerror "Err0718"

func toto()
{
    #if false: enum AA {}
    var x: AA
}

#test
{
    toto()
}