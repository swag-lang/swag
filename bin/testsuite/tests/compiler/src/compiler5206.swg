#global testerror "Saf0019"

#[Swag.Safety(.All, .Null, true)]
func toto() throw
{
    throw @err
}

#test
{
    catch toto()
}
