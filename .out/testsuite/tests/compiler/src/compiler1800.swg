const A = plus(4, 5) + minus(5, 4)
#assert A == 10

#[Swag.ConstExpr]
func plus2(xx, yy: s32) => plus(xx, yy)
const B = plus2(10, 5)
#assert B == 15

#[Swag.ConstExpr]
func plus(xx, yy: s32) => xx + yy
#[Swag.ConstExpr]
func minus(xx, yy: s32) => xx - yy
