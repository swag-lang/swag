#global testerror

func toto()
{
}

//#[swag.printbc]
#test
{
    #[swag.canthrow]
    var x: func()

    x = &toto
    x()
}