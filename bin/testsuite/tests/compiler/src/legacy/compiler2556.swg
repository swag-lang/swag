#global testerror "Err0103"

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
