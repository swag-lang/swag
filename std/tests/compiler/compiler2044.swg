#[swag.inline]
func titi()->{a: f32, b: string}
{
    return @{666, "yes"}
}

//#[swag.printbc]
#test
{
    @assert(titi().a == 666)
    @assert(titi().b == "yes")
}