//#[swag.constexpr]
func toto() => 0
func titi() => 555

//#[swag.printbc]
#test
{
    x := toto() ?? titi()
    @assert(x == 555)
}