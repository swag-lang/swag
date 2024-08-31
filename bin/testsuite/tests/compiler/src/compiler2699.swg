#global testerror "Err0541"
#global testerror "Err0079"
#global testerror "Err0030"

#[Swag.Inline]
func titi()
{
    toto()
}

#[Swag.Inline]
func toto()
{
    titi()
}

//#[Swag.PrintBc]
#test
{
    toto()
}