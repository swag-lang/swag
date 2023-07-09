#[Swag.Optim("", false)]
func toto(y: u32)
{
    {
        var x = 2'u32
        @assert(x >> y == 0)
    }
}

#test
{
    toto(33)
}