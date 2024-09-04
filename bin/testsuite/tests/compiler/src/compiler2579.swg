#global testerror "Err0518"

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