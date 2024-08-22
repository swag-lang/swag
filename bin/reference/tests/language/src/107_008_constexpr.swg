/**
# `Swag.ConstExpr` Functions
A function marked with `Swag.ConstExpr` can be executed at compile time by the compiler if possible. This allows the function's result to be "baked" into the code, reducing runtime computation and potentially optimizing the performance.
*/

#[Swag.ConstExpr]
func sum(x, y: f32) => x + y // Function can be executed at compile time

/**
# Example: Compile-Time Computation
In the example below, the compiler will execute the `sum` function at compile time and bake the result into the constant `G`. The value of `G` will be `3`, computed during the compilation process.
*/

const G = sum(1, 2)
#assert G == 3 // The result of `sum(1, 2)` is computed at compile time and verified

/**
# Forcing Compile-Time Execution with `#run`
If a function is not marked with `Swag.ConstExpr`, but you still want to execute it at compile time, you can use the `#run` directive. This forces the compiler to execute the function during compilation and use the result in your code.
*/

func mul(x, y: f32) => x * y // Normal function, not marked as `Swag.ConstExpr`

// The `#run` directive forces the compile-time execution of `mul(3, 6)`.
const G1 = #run mul(3, 6)
#assert G1 == 18 // The result of `mul(3, 6)` is computed at compile time and verified
