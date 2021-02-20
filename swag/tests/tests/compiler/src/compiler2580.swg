func toto()
{
}

//#[swag.printbc]
#test
{
    #[swag.raiseerrors]
    var x: func()

    x = &toto
    try x()
}