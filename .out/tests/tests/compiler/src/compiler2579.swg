#global testerror

func toto()
{
}

//#[Swag.printbc]
#test
{
    var x: func() throw

    x = &toto
    x()
}