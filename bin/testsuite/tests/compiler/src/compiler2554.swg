#global testerror "Err0746"

func toto()
{
    #if false: struct AA {}
    var x: AA
}

#test
{
    toto()
}