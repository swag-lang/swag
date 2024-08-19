#global testerror "Err0725"

func toto()
{
    #if false: struct AA {}
    var x: AA
}

#test
{
    toto()
}