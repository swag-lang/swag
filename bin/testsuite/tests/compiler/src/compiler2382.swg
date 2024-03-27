//#[Swag.ConstExpr]
func toto() => 0
func titi() => 555

//#[Swag.PrintBc]
#test
{
    let x = toto() orelse titi()
    @assert(x == 555)
}
