#global testerror "Saf0019"

#[Swag.Safety(.All, "null", true)]
func toto() throw
{
    throw @err
}

#test
{
    catch toto()
}
