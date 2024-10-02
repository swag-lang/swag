//#[Swag.PrintBc]
func toto()->nl any
{
    return null
}

//#[Swag.PrintBc]
#test
{
    let x = toto()
    @assert(x == null)
}