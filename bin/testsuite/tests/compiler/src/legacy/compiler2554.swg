#global testerror "Err0103"

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
