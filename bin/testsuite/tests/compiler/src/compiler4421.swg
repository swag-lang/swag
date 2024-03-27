var G = 0

#[Swag.Overload]
func toto(x: &&s32)
{
    @assert(false)
}

#[Swag.Overload]
func toto(x: &s32)
{
    G = 1
}

#test
{
    var x = 0
    toto(&x)
    @assert(G == 1)
}
