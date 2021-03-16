#[swag.constexpr]
func toto() => @{1's64, 2's64}

#[swag.printbc]
#test
{
    x := toto()
    @print(x.item0)
}
