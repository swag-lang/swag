#global testerror

#[Swag.inline]
func titi()
{
    toto()
}

#[Swag.inline]
func toto()
{
    titi()
}

//#[Swag.PrintBc]
#test
{
    toto()
}