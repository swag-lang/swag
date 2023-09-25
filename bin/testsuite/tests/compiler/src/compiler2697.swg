#global testerror "Err0775"
#global testerror "Err0536"

#[Swag.Inline]
func toto()
{
    toto()
}

//#[Swag.PrintBc]
#test
{
    toto()
}
