#[Swag.constexpr]
func plus(xx, yy: s32) => xx + yy
#[Swag.constexpr]
func plus3(xx1, yy1: s32) => plus(xx1, yy1 + 3)
const C = plus3(10, 5)
#assert C == 18