#[Swag.Optimize(.All, false)]
func toto(y: u32)
{
    {
        let x = 2'u32
        @assert(x >> y == 0)
    }
}

#test
{
    toto(33)
}
