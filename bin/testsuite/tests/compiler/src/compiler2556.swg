#global testerror "Err0165"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}