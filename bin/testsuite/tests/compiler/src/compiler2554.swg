#global testerror "Err0728"

func toto()
{
    #if false: struct AA {}
    var x: AA
}

#test
{
    toto()
}