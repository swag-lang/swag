#global testerror

#[Swag.inline]
func titi()
{
    toto()
}

func toto()
{
    titi()
}

//#[Swag.printbc]
#test
{
    toto()
}