#global testerror "Err0718"

func toto()
{
    #if false #do struct AA {}
    var x: AA
}

#test
{
    toto()
}