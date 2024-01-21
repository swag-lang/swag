#global testerror "Err0730"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}