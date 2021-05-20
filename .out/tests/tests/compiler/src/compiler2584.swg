#global testerror

func toto() throw
{
}

//#[Swag.printbc]
#test
{
    var x: func()
    x = &toto
    try x()
}