#[swag.constexpr]
func toto() => 666
func titi() => 666

//#[swag.printbc]
#test
{
    x := toto() + titi()
}