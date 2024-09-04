#global testerror "Err0683"

func toto()
{
    #if false: enum AA {}
    var x: AA
}

#test
{
    toto()
}