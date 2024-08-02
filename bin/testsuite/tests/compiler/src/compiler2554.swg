#global testerror "Err0719"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}