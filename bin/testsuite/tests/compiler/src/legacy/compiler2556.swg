#global testerror "Err0694"

func toto()
{
    #if false: enum AA {}
    var x: AA
}

#test
{
    toto()
}