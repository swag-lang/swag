//#[Swag.PrintBc]
func toto(x: closure() = null)
{
    var y: closure()
    y = x
    if y: y()
}

//#[Swag.PrintBc]
#test
{
    toto()
}