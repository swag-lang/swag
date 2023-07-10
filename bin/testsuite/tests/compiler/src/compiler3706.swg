//#[Swag.PrintBc]
func toto()->any
{
    return null
}

//#[Swag.PrintBc]
#test
{
    let x = toto()
    @assert(x == null)
}