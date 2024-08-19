#global testerror "Err0728"

func toto()
{
    #if false: enum AA {}
    var x: AA
}

#test
{
    toto()
}