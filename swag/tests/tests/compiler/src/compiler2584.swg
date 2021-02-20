#global testerror

func toto() throw
{
}

//#[swag.printbc]
#test
{
    var x: func()
    x = &toto
    try x()
}