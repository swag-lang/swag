// You can use special type 'alias' to create a named alias
// for an identifier
#[Swag.Mixin]
func inc10()
{
    let #alias0 = 10
}

#test
{
    //var a, b = 0
    inc10(|a|)
    inc10(|b|)
    @assert(a == b and b == 10)
}
