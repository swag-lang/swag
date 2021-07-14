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

//#[Swag.PrintBc]
#test
{
    toto()
}