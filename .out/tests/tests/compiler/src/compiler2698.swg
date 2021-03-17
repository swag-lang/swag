#global testerror

#[swag.inline]
func titi()
{
    toto()
}

func toto()
{
    titi()
}

//#[swag.printbc]
#test
{
    toto()
}