using Swag
struct ErrToto {}

#[Inline]
func toto()->s32 throw
{
    throw ErrToto{}
    return 10
}

//#[PrintBc, optim("", false)]
func titi()
{
    var x = catch toto()
    G = 1
}

var G: u32

#test
{
    titi()
    @assert(G == 1)
}
