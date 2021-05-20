var G = 0

#[Swag.inline]
func titi() throw
{
    defer G += 1
    throw "titi"
}

#[Swag.inline]
func tata() throw
{
    defer G += 2
    try titi()
}

#[Swag.inline]
func toto() throw
{
    defer G += 4
    try tata()
}

//#[Swag.printbc]
#test
{
    catch toto()
    //@print(cast(s64) G)
    @assert(G == 7)
}