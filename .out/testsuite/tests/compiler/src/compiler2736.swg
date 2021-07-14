var G = 0

#[Swag.Inline]
func titi() throw
{
    defer G += 1
    throw "titi"
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
    //@print(cast(s64) G)
    @assert(G == 7)
}