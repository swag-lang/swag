
#global testerror

#[Swag.inline]
func toto()
{
    `x = 666
}

#test
{
    x := 0
    toto()
}