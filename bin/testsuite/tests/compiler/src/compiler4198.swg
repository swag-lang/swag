#[Swag.Inline]
func toto()
{
    x := @nameof(toto)
    @assert(x == "toto")
}

#test
{
    toto()
}