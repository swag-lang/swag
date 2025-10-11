//#[Swag.PrintBc]
func toto(y: u32)
{
    var z = y
    z <<= 33
    @assert(z == 0)
}

#test
{
    toto(1)
}
