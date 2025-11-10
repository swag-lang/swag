//#[Swag.PrintBc]
func toto(y: u32)
{
    let z = y << 33
    @assert(z == 0)
}

#test
{
    toto(1)
}
