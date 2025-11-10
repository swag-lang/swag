var G = 0

struct ErrTiti {}

#[Swag.Inline]
func titi() throw
{
    defer G += 1
    throw ErrTiti{}
}

#[Swag.Inline]
func tata() throw
{
    defer G += 2
    try titi()
}

#[Swag.Inline]
func toto() throw
{
    defer G += 4
    try tata()
}

//#[Swag.PrintBc]
#test
{
    catch toto()
    @assert(G == 7)
}
