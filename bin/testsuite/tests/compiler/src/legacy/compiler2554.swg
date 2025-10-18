#global testerror "Err0105"

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
