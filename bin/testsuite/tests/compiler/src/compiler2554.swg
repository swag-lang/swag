#global testerror "Err0165"

func toto()
{
    #if false struct AA {}
    var x: AA
}

#test
{
    toto()
}