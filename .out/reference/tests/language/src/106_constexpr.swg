// A function marked with 'Swag.constexpr' can be executed by the compiler if it can
#[Swag.constexpr]
func sum(x, y: f32) => x + y

// Here 'G' will be baked to 3 by the compiler
const G = sum(1, 2)
#assert G == 3

// If a function is not 'constexpr', you can force the compile time call with #run
func mul(x, y: f32) => x * y
const G1 = #run mul(3, 6)
#assert G1 == 18