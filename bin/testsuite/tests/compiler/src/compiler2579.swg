#global testerror "Err0648"

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