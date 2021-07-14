#[Swag.ConstExpr]
func plus(x: u32) => x == 10
const C = plus(10)
#assert C == true