func toto()
{
}

//#[swag.printbc]
#test
{
    var x: func()

    x = &toto
    try x()
}