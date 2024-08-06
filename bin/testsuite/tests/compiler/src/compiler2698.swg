#global testerror "Err0563"

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