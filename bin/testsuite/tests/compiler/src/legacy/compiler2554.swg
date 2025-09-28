#global testerror "Err0694"

func toto()
{
    #if false: struct AA {}
    var x: AA
}

#test
{
    toto()
}