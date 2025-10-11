#global testerror "Err0694"

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
