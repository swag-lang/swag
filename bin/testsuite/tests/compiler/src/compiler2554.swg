#global testerror "Err0730"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}