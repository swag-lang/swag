/**
# `Swag.ConstExpr` Functions

A function marked with `Swag.ConstExpr` can be executed at compile time by the compiler if 
the input values are known at that stage. This allows the function's result to be "baked" 
into the code, reducing runtime computation and potentially improving performance.
*/

#[Swag.ConstExpr]
func sum(x, y: f32) => x + y     // This function can be executed at compile time.

/**
# Example: Compile-Time Computation

In the example below, the compiler will execute the `sum` function at compile time, 
embedding the result directly into the constant `G`. The value of `G` will be `3`, 
computed during the compilation process, ensuring no runtime overhead for this calculation.
*/

const G = sum(1, 2)
#assert G == 3                   // The result of `sum(1, 2)` is computed at compile time and verified.

/**
# Example: Using `Swag.ConstExpr` with Complex Expressions

`Swag.ConstExpr` functions can handle more complex expressions as well. The following example 
shows how a compile-time function can perform multiple operations and still have its result 
computed during compilation.
*/

#[Swag.ConstExpr]
func complexCalc(a, b, c: f32) => (a + b) * c / 2

const result = complexCalc(4, 5, 6)
#assert result == 27.0           // The complex expression is evaluated at compile time.

/**
# Example: Compile-Time Execution of Array Initializations

`Swag.ConstExpr` functions can also be used to initialize arrays or other data structures at 
compile time. This example demonstrates how an array of precomputed values is created.
*/

#[Swag.ConstExpr]
func square(n: s32) => n * n

const Squares = [square(1), square(2), square(3), square(4), square(5)]
// #assert Squares[1] == 1
// #assert Squares[1] == 4
// #assert Squares[2] == 9
// #assert Squares[3] == 16
// #assert Squares[4] == 25

/**
# Forcing Compile-Time Execution with `#run`

If a function is not marked with `Swag.ConstExpr`, but you still want to execute it at compile time, 
you can use the `#run` directive. This forces the compiler to execute the function during 
compilation and use the result in your code.
*/

func mul(x, y: f32) => x * y      // This is a normal function, not marked as `Swag.ConstExpr`.

// The `#run` directive forces the compile-time execution of `mul(3, 6)`.
const G1 = #run mul(3, 6)
#assert G1 == 18                  // The result of `mul(3, 6)` is computed at compile time and verified.

/**
# Example: Compile-Time Evaluation of Conditional Logic

Using `Swag.ConstExpr` or `#run`, you can evaluate conditional logic at compile time, 
enabling the embedding of decision results directly into your constants.
*/

#[Swag.ConstExpr]
func max(a, b: s32) => a > b ? a : b

const MaxValue = max(10, 20)
#assert MaxValue == 20            // The comparison is performed at compile time.

/**
# Example: Compile-Time Initialization of Structs

You can initialize complex data structures, such as structs, at compile time using `Swag.ConstExpr`. 
This ensures that the structure's values are determined before runtime.
*/

#[Swag.ConstExpr]
struct Point { x, y: s32 }

#[Swag.ConstExpr]
func createPoint(a, b: s32) => Point{a, b}

const Origin = createPoint(1, 2)
#assert Origin.x == 1 and Origin.y == 2  // The Point is initialized at compile time.

/**
# Example: Using `#run` with User-Defined Types

In cases where `Swag.ConstExpr` is not used, `#run` can force compile-time execution even 
for user-defined types like structs or enums.
*/

struct Rectangle { width, height: s32 }

func area(rect: Rectangle) => rect.width * rect.height

const RectStatic = Rectangle{ 5, 10 }
const RectArea = #run area(RectStatic)
#assert RectArea == 50            // The area of the rectangle is computed at compile time.
