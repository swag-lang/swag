using Swag
struct ErrToto {}

#[Inline]
func toto()->s32 throw
{
    throw ErrToto{}
    return 10
}

//#[PrintBc, optim("", false)]
func titi() throw
{
    var x = try toto()
    G = 1
}

var G: u32

#test
{
    catch titi()
    @assert(G == 0)
}
