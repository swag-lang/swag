//#[Swag.PrintBc]
func toto()->any
{
    return null
}

//#[Swag.PrintBc]
#test
{
    var x = toto()
    @assert(x == null)
}