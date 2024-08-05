
#global testerror "Err0442"

#[Swag.Inline]
func toto()
{
    #up x = 666
}

#test
{
    var x = 0
    toto()
}