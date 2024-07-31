#global testerror "Err0735"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}