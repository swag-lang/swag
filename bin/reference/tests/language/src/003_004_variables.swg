/**
# Variable Declaration
To declare a variable in Swag, you use the `let` or `var` keyword, followed by a `:` and then the type.

- `let` is used for a variable that cannot be changed after its initial assignment. It's immutable.
- `var` is used for a variable that can be changed after its initial assignment. It's mutable.
*/
#test
{
    // 'a' is a constant variable of type 'u32' and is initialized with the value 1. It cannot be modified later.
    let a: u32 = 1
    @assert(a == 1)

    // 'b' is a constant variable of type 'string'.
    let b: string = "string"
    @assert(b == "string")

    // 'c' is a mutable variable of type 's32', so its value can be changed.
    var c: s32 = 42
    c += 1
    @assert(c == 43)  // Verifies that 'c' has been correctly incremented.
}

/**
# Multiple Variable Declarations
We can also declare multiple variables on the same line.
*/
#test
{
    let a, b: u32 = 123  // Both 'a' and 'b' are of type 'u32' and are initialized with the value 123.
    @assert(a == 123)
    @assert(b == 123)
}

/**
Alternatively, you can declare variables of different types on the same line.
*/
#test
{
    let a: u32 = 12, b: f32 = 1.5  // 'a' is of type 'u32' and 'b' is of type 'f32'.
    @assert(a == 12)
    @assert(b == 1.5)
}

/**
# Default Initialization
If you don't assign an initial value, the variable will be automatically initialized with its default value. Therefore, a variable in Swag is **always** initialized.
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
However, if you want a variable to remain uninitialized (and avoid the cost of initialization), you can assign it `undefined`. This should be used with caution as it leaves the variable in an undefined state.
*/
#test
{
    var a: bool = undefined  // 'a' is left uninitialized.
    var b: string = undefined  // 'b' is left uninitialized.
}

/**
# Type Inference
As we have seen, the type in a declaration is optional if it can be deduced from the assigned value. This feature is known as **type inference**.
*/

/**
Here are some examples of **type inference**.
*/
#test
{
    let a = 1.5  // The type of 'a' is inferred to be 'f32' because 1.5 is a floating-point literal.
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
Type inference also works when declaring multiple variables.
*/
#test
{
    let a, b = true  // Both 'a' and 'b' are inferred to be of type 'bool'.
    @assert(a == true)
    @assert(b == true)
    #assert @typeof(a) == @typeof(true)
    #assert @typeof(b) == @typeof(a)

    let c = 1.5, d = "string"  // 'c' is inferred to be 'f32', and 'd' is inferred to be 'string'.
    @assert(c == 1.5)
    @assert(d == "string")
    #assert @typeof(c) == f32
    #assert @typeof(d) == string
}

/**
# Special Variables
Swag provides special keywords and attributes to manage variable storage and behavior beyond the typical scope.

## Thread-Local Storage
A global variable can be tagged with `#[Swag.Tls]` to store it in thread-local storage, meaning each thread will have its own copy of the variable.
*/
#[Swag.Tls]
var G = 0  // 'G' is a global variable with thread-local storage.

 /**
## Global Variables
A local variable can be tagged with `#[Swag.Global]` to make it global, similar to the `static` keyword in C/C++.
*/
#test
{
    func toto()->s32
    {
        #[Swag.Global]
        var G1 = 0  // 'G1' is a static-like variable, retaining its value between function calls.

        G1 += 1
        return G1
    }

    @assert(toto() == 1)  // First call increments G1 to 1.
    @assert(toto() == 2)  // Second call increments G1 to 2.
    @assert(toto() == 3)  // Third call increments G1 to 3.
}

/**
## Compile-Time Variables
A global variable can also be marked with `#[Swag.Compiler]`. Such variables are only accessible during compile-time and are not included in the runtime code.
*/
#[Swag.Compiler]
var G2 = 0  // 'G2' is a compile-time only variable.

#run
{
    G2 += 5  // This increment happens at compile-time.
}
