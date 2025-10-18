#global testerror "Err0105"

func toto()
{
    #if false do
        enum AA {  }
    var x: AA
}

#test
{
    toto()
}
