// You can use special type 'typealias' to create a named typealias
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