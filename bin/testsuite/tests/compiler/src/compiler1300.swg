
#global testerror

#[Swag.Inline]
func toto()
{
    #up x = 666
}

#test
{
    x := 0
    toto()
}