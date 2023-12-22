#global testerror "Err0165"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}