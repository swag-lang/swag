#[Swag.Macro]
func toto(x, y: s64 = 666, stmt: code)
{
    @assert(x == 444)
    @assert(y == 666)
    #mixin stmt
}

#test
{
    toto(x: 444)
    {
        @assert(x == 444)
        @assert(y == 666)
    }
}
