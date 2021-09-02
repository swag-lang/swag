func toto() throw
{
}

//#[Swag.PrintBc]
#test
{
    var x: func() throw

    x = &toto
    try x()
}