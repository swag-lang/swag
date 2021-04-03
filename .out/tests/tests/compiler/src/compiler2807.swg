var G = 0

//#[swag.printbc]
func toto(y: *u8, x: string)
{
    concatLength := @countof(x)
    if concatLength == 0
        return
    G = 1
}

#test
{
    toto(null, "X")
    @assert(G == 1)
}