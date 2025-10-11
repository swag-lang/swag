#[Swag.Overload]
func toto(x: s32) {}

#[Swag.Inline, Swag.Overload]
func toto()
{
    let x = #nameof(toto)
    @assert(x == "toto")
}

#test
{
    toto()
}
