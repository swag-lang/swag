// You can use special type 'alias' to create a named alias
// for an identifier
#[Swag.Mixin]
func inc10(variable: alias)
{
    variable := 10
}

#test
{
    //a, b := 0
    inc10(#code a)
    inc10(#code b)
    @assert(a == b and b == 10)
}