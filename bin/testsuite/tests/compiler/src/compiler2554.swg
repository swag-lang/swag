#global testerror "Err0727"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}