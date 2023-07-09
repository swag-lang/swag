#[Swag.Inline]
func toto()
{
    var x = @nameof(toto)
    @assert(x == "toto")
}

#test
{
    toto()
}