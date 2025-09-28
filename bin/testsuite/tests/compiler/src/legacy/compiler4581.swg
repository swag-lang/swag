//#[Swag.PrintBc]
func toto(x: func||() = null)
{
    var y: func||()
    y = x
    if y: y()
}

//#[Swag.PrintBc]
#test
{
    toto()
}