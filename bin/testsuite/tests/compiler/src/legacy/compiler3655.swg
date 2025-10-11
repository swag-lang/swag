#[Swag.Inline, Swag.ConstExpr]
func toto()->s32
{
    var a: [10] s32
    a[0] = 666
    return a[0]
}

var y    = toto()
const y1 = toto()
#assert(toto() == 666)
#assert(toto() + 1 == 667)
#assert(toto() + toto() == 666 + 666)
