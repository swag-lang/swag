//#[Swag.ConstExpr]
func toto() => 0
func titi() => 555

//#[Swag.PrintBc]
#test
{
    x := toto() orelse titi()
    @assert(x == 555)
}