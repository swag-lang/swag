#global testerror

#[swag.inline]
func toto()
{
    toto()
}

//#[swag.printbc]
#test
{
    toto()
}