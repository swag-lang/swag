#global testerror

#[swag.inline]
func titi()
{
    toto()
}

#[swag.inline]
func toto()
{
    titi()
}

//#[swag.printbc]
#test
{
    toto()
}