func toto(x: s32)
{
}

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