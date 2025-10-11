var G = 0

//#[Swag.PrintBc]
func toto(y: #null *u8, x: string)
{
    let concatLength = @countof(x)
    if concatLength == 0 do
        return
    G = 1
}

#test
{
    toto(null, "X")
    @assert(G == 1)
}
