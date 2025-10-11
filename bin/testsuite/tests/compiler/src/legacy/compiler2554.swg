#global testerror "Err0694"

func toto()
{
    #if false do
        struct AA {}
    var x: AA
}

#test
{
    toto()
}
