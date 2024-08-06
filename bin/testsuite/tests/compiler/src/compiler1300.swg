
#global testerror "Err0391"

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