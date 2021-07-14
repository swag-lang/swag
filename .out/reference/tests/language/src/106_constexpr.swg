// A function marked with 'Swag.ConstExpr' can be executed by the compiler if it can
#[Swag.ConstExpr]
func sum(x, y: f32) => x + y

// Here 'G' will be baked to 3 by the compiler
const G = sum(1, 2)
#assert G == 3

// If a function is not 'ConstExpr', you can force the compile time call with #run
func mul(x, y: f32) => x * y
const G1 = #run mul(3, 6)
#assert G1 == 18