var G = 0

#[Swag.Overload]
func toto(x: &&s32)
{
    G = 1
}

#[Swag.Overload]
func toto(x: &s32)
{
    @assert(false)
}

#test
{
    var x = 0
    toto(moveref &x)
    @assert(G == 1)
}
