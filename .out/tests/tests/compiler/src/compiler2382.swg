//#[Swag.constexpr]
func toto() => 0
func titi() => 555

//#[Swag.printbc]
#test
{
    x := toto() orelse titi()
    @assert(x == 555)
}