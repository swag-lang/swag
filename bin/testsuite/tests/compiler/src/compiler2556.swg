#global testerror "Err0717"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}