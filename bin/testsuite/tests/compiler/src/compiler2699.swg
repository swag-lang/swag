#global testerror "Err0775"
#global testerror "Err0015"
#global testerror "Err0536"

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