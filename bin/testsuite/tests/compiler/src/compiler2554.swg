#global testerror "Err0718"

func toto()
{
    #if false: struct AA {}
    var x: AA
}

#test
{
    toto()
}