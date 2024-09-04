#global testerror "Err0683"

func toto()
{
    #if false: struct AA {}
    var x: AA
}

#test
{
    toto()
}