#global testerror "Err0715"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}