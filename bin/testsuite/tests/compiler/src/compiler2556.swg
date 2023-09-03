#global testerror "Err0165"

func toto()
{
    #if false enum AA {}
    var x: AA
}

#test
{
    toto()
}