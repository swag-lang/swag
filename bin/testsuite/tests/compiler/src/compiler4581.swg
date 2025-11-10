//#[Swag.PrintBc]
func toto(x: func||() = null)
{
    var y: func||()
    y = x
    if y do
        y()
}

//#[Swag.PrintBc]
#test
{
    toto()
}
