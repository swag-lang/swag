#global testerror "Err0718"

func toto()
{
    #if false #do enum AA {}
    var x: AA
}

#test
{
    toto()
}