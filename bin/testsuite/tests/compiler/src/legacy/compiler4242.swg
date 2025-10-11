#[Swag.ConstExpr]
func toto()->typeinfo => #typeof(s32)

var G = 0
#test
{
    #if toto() == s32
    {
        G = 1
    }

    @assert(G == 1)
}
