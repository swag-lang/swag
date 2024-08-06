#global testerror "Err0599"

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