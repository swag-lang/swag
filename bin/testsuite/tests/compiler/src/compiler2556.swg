#global testerror "Err0725"

func toto()
{
    #if false: enum AA {}
    var x: AA
}

#test
{
    toto()
}