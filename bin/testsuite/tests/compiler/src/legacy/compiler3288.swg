/*#[Swag.Mixin]
func MK_IMPL()
{
    #ast
    {
        return "TOTO = 10; TITI = 11;"
    }
}

enum X
{
    MK_IMPL()
}

#test
{
    var v: X = TOTO
    @assert(v == 10)
    v = .TITI
    @assert(v == 11)
}*/
