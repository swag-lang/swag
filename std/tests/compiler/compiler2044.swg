#[swag.inline]
func titi()->{a: f32}
{
    return @{666}
}

//#[swag.printbc]
#test
{
    @assert(titi().a == 666)
}