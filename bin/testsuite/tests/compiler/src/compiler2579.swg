#global testerror "Err0599"

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