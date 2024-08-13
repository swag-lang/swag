#global testerror "Err0715"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}