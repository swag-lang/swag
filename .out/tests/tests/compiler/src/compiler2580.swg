func toto() throw
{
}

//#[swag.printbc]
#test
{
    var x: func() throw

    x = &toto
    try x()
}