#global testerror "Err0153"

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
