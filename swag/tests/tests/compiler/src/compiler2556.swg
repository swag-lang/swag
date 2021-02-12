#global testerror

func toto()
{
    #if false enum AA {}
    var x: AA
}

#test
{
    toto()
}