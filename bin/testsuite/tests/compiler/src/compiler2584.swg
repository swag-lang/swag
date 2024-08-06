#global testerror "Err0592"

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