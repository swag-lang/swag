#global testerror "Err0501"
#global testerror "Err0077"
#global testerror "Err0027"
#global testerror "Err0515"

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