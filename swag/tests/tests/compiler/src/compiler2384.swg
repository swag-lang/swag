//#[swag.constexpr]
func toto() => 0'u8
func titi() => 55'u8

#[swag.printbc]
#test
{
    x := toto() ?? titi()
    @assert(x == 55)
}