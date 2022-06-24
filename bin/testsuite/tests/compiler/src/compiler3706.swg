//#[Swag.PrintBc]
func toto()->any
{
    return null
}

//#[Swag.PrintBc]
#test
{
    x := toto()
    @assert(x == null)
}