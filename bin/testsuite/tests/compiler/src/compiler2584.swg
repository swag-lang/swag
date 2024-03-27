#global testerror "Err0643"

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