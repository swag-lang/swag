var G = 0

//#[Swag.PrintBc]
func toto()
{
    x := 0
    if x == 0
        return
    defer G = 1
}

#test
{
    toto()
    @assert(G == 0)
}