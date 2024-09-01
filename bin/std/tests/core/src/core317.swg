using Core

struct X
{
    a, b, c: s32
}

#[Swag.Compiler, Swag.ConstExpr]
func(T) GEN()->String
{
    var sb: StrConv.StringBuilder
    sb.appendFormat("X", #nameof(T))
    return sb.toString()
}

#assert cast(string) GEN'X() == "X"
#assert GEN'X()[0] == `X`'u8
