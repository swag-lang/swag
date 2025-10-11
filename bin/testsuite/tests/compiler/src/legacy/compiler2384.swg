//#[Swag.ConstExpr]
func toto() => 0'u8
func titi() => 55'u8

//#[Swag.PrintBc]
#test
{
    let x = toto() orelse titi()
    @assert(x == 55)
}
