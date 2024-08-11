#global testerror "Err0709"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}