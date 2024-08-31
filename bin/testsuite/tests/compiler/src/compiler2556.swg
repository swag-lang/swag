#global testerror "Err0746"

func toto()
{
    #if false: enum AA {}
    var x: AA
}

#test
{
    toto()
}