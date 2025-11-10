var G = 0

#[Swag.Macro]
func(op: bool) toto()
{
    #if op do
        G += 1
    #else do
        G += 2
}

//#[Swag.PrintBc]
#test
{
    G = 0
    toto'true()
    toto'false()
    @assert(G == 3)
}
