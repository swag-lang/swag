#global testerror "Err0151"

#[Swag.Inline]
func toto()
{
    //@print(x)
}

//#[Swag.PrintBc]
#test
{
    var x = &toto
}
