#global testerror

func toto()
{
    #if false struct AA {}
    var x: AA
}

#test
{
    toto()
}