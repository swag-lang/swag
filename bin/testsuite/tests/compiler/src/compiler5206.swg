#global testerror "Saf0036"

#[Swag.Safety("null", true)]
func toto() throw
{
    throw @err()
}

#test
{
    catch toto()
}