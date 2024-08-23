/**
# Variable Declaration
In Swag, variables are declared using the `let` or `var` keyword, followed by a `:` and the variable's type.

- **`let`**: Used for declaring a constant variable. The value assigned cannot be modified, ensuring immutability.
- **`var`**: Used for declaring a mutable variable. The value assigned can be modified after initialization.
*/

#test
{
    // 'a' is a constant variable of type 'u32', initialized with the value 1.
    // It is immutable, meaning its value cannot be altered after assignment.
    let a: u32 = 1
    @assert(a == 1)

    // 'b' is a constant variable of type 'string', initialized with the value "string".
    let b: string = "string"
    @assert(b == "string")

    // 'c' is a mutable variable of type 's32'. Its value is initialized to 42 and can be modified later.
    var c: s32 = 42
    c += 1
    @assert(c == 43)  // Verifies that 'c' has been correctly incremented to 43.
}

/**
# Multiple Variable Declarations
Swag allows the declaration of multiple variables of the same type on a single line.
*/
#test
{
    let a, b: u32 = 123  // Both 'a' and 'b' are of type 'u32', initialized with the value 123.
    @assert(a == 123)
    @assert(b == 123)
}

/**
Alternatively, multiple variables of different types can also be declared on the same line.
*/
#test
{
    let a: u32 = 12, b: f32 = 1.5  // 'a' is of type 'u32', and 'b' is of type 'f32'.
    @assert(a == 12)
    @assert(b == 1.5)
}

/**
# Default Initialization
In Swag, if a variable is declared without an initial value, it is automatically initialized with its default value.
A variable is **always** initialized, ensuring it never holds an undefined value.
*/
#test
{
    var a: bool
    @assert(a == false)  // The default value for a boolean is 'false'.

    var b: string
    @assert(b == null)  // The default value for a string is 'null'.

    var c: f64
    @assert(c == 0)  // The default value for a floating-point number is 0.
}

/**
# Uninitialized Variables
If you want a variable to remain uninitialized and avoid the default initialization cost, you can assign it `undefined`.
This approach should be used with caution, as the variable will be in an undefined state.
*/
#test
{
    var a: bool = undefined  // 'a' is intentionally left uninitialized.
    var b: string = undefined  // 'b' is intentionally left uninitialized.
}

/**
# Type Inference
Swag supports type inference, where the type of a variable can be automatically deduced from its assigned value.
This allows the omission of explicit type annotations in many cases.
*/

/**
Below are examples demonstrating **type inference**.
*/
#test
{
    let a = 1.5  // The type of 'a' is inferred to be 'f32' due to the floating-point literal.
    @assert(a == 1.5)
    #assert @typeof(a) == f32

    let b = "string"  // The type of 'b' is inferred to be 'string'.
    @assert(b == "string")
    #assert @typeof(b) == string

    let c = 1.5'f64  // The type of 'c' is explicitly set to 'f64' using a suffix.
    @assert(c == 1.5)
    #assert @typeof(c) == f64
}

/**
Type inference also applies when declaring multiple variables simultaneously.
*/
#test
{
    let a, b = true  // Both 'a' and 'b' are inferred to be of type 'bool'.
    @assert(a == true)
    @assert(b == true)
    #assert @typeof(a) == @typeof(true)
    #assert @typeof(b) == @typeof(a)

    let c = 1.5, d = "string"  // 'c' is inferred as 'f32', and 'd' as 'string'.
    @assert(c == 1.5)
    @assert(d == "string")
    #assert @typeof(c) == f32
    #assert @typeof(d) == string
}

/**
# Special Variables
Swag offers special keywords and attributes to manage variable storage and behavior beyond typical usage.

## Thread-Local Storage
By tagging a global variable with `#[Swag.Tls]`, you can store it in thread-local storage. 
Each thread will then have its own independent copy of the variable.
*/
#[Swag.Tls]
var G = 0  // 'G' is a global variable stored in thread-local storage.

/**
## Global Variables
A local variable can be tagged with `#[Swag.Global]` to make it global, functioning similarly to the `static` keyword in C/C++.
*/
#test
{
    func toto() -> s32
    {
        #[Swag.Global]
        var G1 = 0  // 'G1' is a static-like variable that retains its value across function calls.

        G1 += 1
        return G1
    }

    @assert(toto() == 1)  // First call increments G1 to 1.
    @assert(toto() == 2)  // Second call increments G1 to 2.
    @assert(toto() == 3)  // Third call increments G1 to 3.
}

/**
## Compile-Time Variables
Global variables marked with `#[Swag.Compiler]` are only accessible during compile-time and are excluded from the runtime code.
*/
#[Swag.Compiler]
var G2 = 0  // 'G2' is a compile-time-only variable.

#run
{
    G2 += 5  // This increment occurs at compile-time.
}
