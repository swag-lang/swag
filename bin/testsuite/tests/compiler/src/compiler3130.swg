var G = 0

#[Swag.Overload]
func whatever(x: s32)
    verify #isconstexpr(x)
{
    G += 1
}

#[Swag.Overload]
func whatever(x: s32)
    verify !#isconstexpr(x)
{
    G += 2
}

#test
{
    whatever(665)
    var x = 664
    x += 1
    whatever(x)
    @assert(G == 3)
}
