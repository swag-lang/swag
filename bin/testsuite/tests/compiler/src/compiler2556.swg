#global testerror "Err0735"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}