#global testerror "Err0720"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}