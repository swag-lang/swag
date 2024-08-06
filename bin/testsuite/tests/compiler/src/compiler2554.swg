#global testerror "Err0720"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}