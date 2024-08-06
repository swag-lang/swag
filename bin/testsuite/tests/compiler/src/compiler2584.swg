#global testerror "Err0630"

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