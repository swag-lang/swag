#global testerror "Err0727"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}