var G = 0

#[Swag.Overload]
func toto(x: &&s32)
    where true
{
    G = 1
}

#[Swag.Overload]
func toto(x: &s32)
    where true
{
    @assert(false)
}

#test
{
    var x = 0
    toto(moveref &x)
    @assert(G == 1)
}
