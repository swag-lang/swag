var G = 0

#[swag.inline]
func titi() throw
{
    defer G += 1
    throw "titi"
}

#[swag.inline]
func tata() throw
{
    defer G += 2
    try titi()
}

#[swag.inline]
func toto() throw
{
    defer G += 4
    try tata()
}

//#[swag.printbc]
#test
{
    catch toto()
    //@print(cast(s64) G)
    @assert(G == 7)
}