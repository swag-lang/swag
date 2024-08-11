#global testerror "Err0709"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}