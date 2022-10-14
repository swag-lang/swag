//#global testerror

using Core

#[Swag.Compiler, Swag.ConstExpr]
func GEN()->String
{
    var sb: StrConv.StringBuilder
    sb.appendFormat("const X = %")
    return sb.toString()
}

/*
#ast cast(string) GEN()
#placeholder X
const Y = X
#assert Y == 555*/