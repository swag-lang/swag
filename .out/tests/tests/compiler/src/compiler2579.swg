#global testerror

func toto()
{
}

//#[swag.printbc]
#test
{
    var x: func() throw

    x = &toto
    x()
}