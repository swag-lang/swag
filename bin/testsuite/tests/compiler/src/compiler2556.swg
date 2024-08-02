#global testerror "Err0719"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}