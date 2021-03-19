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
    defer G += 1
    try titi()
}

#[swag.inline]
func toto() throw
{
    defer G += 1
    try tata()
}

#test
{
    catch toto()
    @assert(G == 3)
}