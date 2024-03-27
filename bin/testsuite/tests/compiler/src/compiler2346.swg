var G = 0

//#[Swag.PrintBc]
func toto()
{
    let x = 0
    if x == 0 do
        return
    defer G = 1
}

#test
{
    toto()
    @assert(G == 0)
}
