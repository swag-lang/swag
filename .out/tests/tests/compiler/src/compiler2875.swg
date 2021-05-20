#[Swag.macro]
func toto(stmt: code)
{
    var @alias0: s64 = 555
    var @alias1: s64 = 666
    #mixin stmt
}

#test
{
    toto(|x, y|)
    {
        @assert(x == 555)
        @assert(y == 666)
    }
}