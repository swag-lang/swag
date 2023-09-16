#global testerror "Err1177"

var (x, y) = toto()

#[Swag.ConstExpr]
func toto() -> {x: s32, y: s32}
{
    return {10, 20}
}