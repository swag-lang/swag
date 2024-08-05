#global testerror "Err0645"

func toto() throw
{
}

//#[Swag.PrintBc]
#test
{
    var x: func()
    x = &toto
    try x()
}