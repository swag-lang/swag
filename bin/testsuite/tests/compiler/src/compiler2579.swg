#global testerror "Err0645"

func toto()
{
}

//#[Swag.PrintBc]
#test
{
    var x: func() throw

    x = &toto
    x()
}