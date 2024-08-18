typealias H = closure()

//#[Swag.PrintBc]
func toto(x: H = null)
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