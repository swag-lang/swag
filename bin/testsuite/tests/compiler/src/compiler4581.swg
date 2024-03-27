//#[Swag.PrintBc]
func toto(x: closure() = null)
{
    var y: closure()
    y = x
    if y do
        y()
}

//#[Swag.PrintBc]
#test
{
    toto()
}
