#global testerror "Err0724"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}