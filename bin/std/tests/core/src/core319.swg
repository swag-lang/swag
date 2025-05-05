//#global testerror

using Core

#[Swag.Compiler, Swag.ConstExpr]
func GEN()->String
{
    var sb: StrConv.StringBuilder
    sb.appendFormat("const X = %")
    return sb.toString()
}
