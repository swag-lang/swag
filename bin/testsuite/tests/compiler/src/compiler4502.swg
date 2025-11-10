#[Swag.Optimize(.All, false)]
func toto(y: s32)
{
    {
        let x = 2's32
        @assert(x >> y == 1)
    }
}

#test
{
    toto(1)
}
