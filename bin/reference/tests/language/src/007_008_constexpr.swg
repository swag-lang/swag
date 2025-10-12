/**
# `Swag.ConstExpr` Functions
Functions marked with '#[Swag.ConstExpr]' can be executed at compile time if their inputs
are known. The compiler computes their results during compilation, embedding the values
directly into the code for zero runtime overhead.
*/
#[Swag.ConstExpr]
func sum(x, y: f32) => x + y

/**
# Example: Compile-Time Computation
The compiler executes 'sum(1, 2)' at compile time, embedding the result directly in 'G'.
*/
const G = sum(1, 2)
#assert(G == 3)

/**
# Example: Using `Swag.ConstExpr` with Complex Expressions
ConstExpr functions can evaluate compound arithmetic expressions at compile time.
*/
#[Swag.ConstExpr]
func complexCalc(a, b, c: f32) => (a + b) * c / 2

const result = complexCalc(4, 5, 6)
#assert(result == 27.0)

/**
# Example: Compile-Time Execution of Array Initializations
ConstExpr functions can initialize arrays and collections at compile time.
*/
#[Swag.ConstExpr]
func square(n: s32) => n * n

const Squares = [square(1), square(2), square(3), square(4), square(5)]
#assert(Squares[0] == 1)
#assert(Squares[1] == 4)
#assert(Squares[2] == 9)
#assert(Squares[3] == 16)
#assert(Squares[4] == 25)

/**
# Forcing Compile-Time Execution with `#run`
'#run' forces compile-time execution even for functions not marked as ConstExpr.
*/
func mul(x, y: f32) => x * y

const G1 = #run mul(3, 6)
#assert(G1 == 18)

/**
# Example: Compile-Time Evaluation of Conditional Logic
You can evaluate conditions and branches at compile time using ConstExpr functions.
*/
#[Swag.ConstExpr]
func max(a, b: s32) => a > b ? a : b

const MaxValue = max(10, 20)
#assert(MaxValue == 20)

/**
# Example: Compile-Time Initialization of Structs
ConstExpr functions can construct and initialize user-defined structs during compilation.
*/
#[Swag.ConstExpr]
struct Point { x, y: s32 }

#[Swag.ConstExpr]
func createPoint(a, b: s32) => Point{a, b}

const Origin = createPoint(1, 2)
#assert(Origin.x == 1 and Origin.y == 2)

/**
# Example: Using `#run` with User-Defined Types
'#run' can be used to execute ordinary functions with structs at compile time.
*/
struct Rectangle { width, height: s32 }

func area(rect: Rectangle) => rect.width * rect.height

const RectStatic = Rectangle{5, 10}
const RectArea   = #run area(RectStatic)
#assert(RectArea == 50)
