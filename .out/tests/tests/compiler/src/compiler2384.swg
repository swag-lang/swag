//#[swag.constexpr]
func toto() => 0'u8
func titi() => 55'u8

//#[swag.printbc]
#test
{
    x := toto() orelse titi()
    @assert(x == 55)
}