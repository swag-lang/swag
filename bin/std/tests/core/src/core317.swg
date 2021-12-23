using Core

struct X
{
    a, b, c: s32
}

#[Swag.Compiler, Swag.ConstExpr]
func(T) GEN()->String
{
    var sb: StrConv.StringBuilder
    sb.appendFormat("X", @nameof(T))
    return sb.moveToString()
}

//#assert GEN'X() == "X"'u8
#assert cast(string) GEN'X() == "X"