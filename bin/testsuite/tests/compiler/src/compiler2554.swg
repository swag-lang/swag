#global testerror "Err0724"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}