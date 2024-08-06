#global testerror "Err0592"

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