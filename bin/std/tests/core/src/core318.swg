using Core

#[Swag.Compiler, Swag.ConstExpr]
func GEN()->String
{
    var sb: StrConv.StringBuilder
    sb.appendString("const X = 555")
    return sb.toString()
}

#ast cast(string) GEN()

#placeholder X
const Y = X
#assert(Y == 555)
