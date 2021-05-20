#[Swag.constexpr]
func toto(x: s32) => x

var G = 0

func(NNN: s32) titi()
{
    #if NNN.toto() == 6
    {
        G = NNN
    }
}

#test
{
    titi'6()
    @assert(G == 6)
}