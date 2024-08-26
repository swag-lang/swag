#global testerror "Err0736"

func toto()
{
    #if false: struct AA {}
    var x: AA
}

#test
{
    toto()
}