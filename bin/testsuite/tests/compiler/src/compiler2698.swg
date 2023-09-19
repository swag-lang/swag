#global testerror "Err0536"

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