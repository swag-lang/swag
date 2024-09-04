#global testerror "Err0026"

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