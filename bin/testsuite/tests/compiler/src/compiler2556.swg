#global testerror "Err0736"

func toto()
{
    #if false: enum AA {}
    var x: AA
}

#test
{
    toto()
}