#global testerror "Err0511"

var (x, y) = toto()

#[Swag.ConstExpr]
func toto() -> {x: s32, y: s32}
{
    return {10, 20}
}