alias H = func||()

//#[Swag.PrintBc]
func toto(x: H = null)
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
