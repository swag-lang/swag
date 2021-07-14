
#global testerror

#[Swag.Inline]
func toto()
{
    `x = 666
}

#test
{
    x := 0
    toto()
}