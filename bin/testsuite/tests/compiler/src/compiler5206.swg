#global testerror "Saf0018"

#[Swag.Safety("null", true)]
func toto() throw
{
    throw @err()
}

#test
{
    catch toto()
}