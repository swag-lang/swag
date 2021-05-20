#[Swag.constexpr]
func toto() => @{1's64, 2's64}

#test
{
    x := toto()
    @assert(x.item0 == 1)
    @assert(x.item1 == 2)
}
