
#unittest error

#[swag.inline]
func toto()
{
    `x = 666
}

#test
{
    x := 0
    toto()
}