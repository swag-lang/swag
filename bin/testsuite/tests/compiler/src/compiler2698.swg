#global testerror "Err0571"

#[Swag.Inline]
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