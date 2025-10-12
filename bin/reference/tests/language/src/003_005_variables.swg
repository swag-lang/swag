/**
# Variable Declaration

Variables are declared using the 'let' or 'var' keyword, followed by a ':' and
the variable’s type.

- **'let'** — Declares an immutable variable. Once assigned, its value cannot change.
- **'var'** — Declares a mutable variable. Its value can be modified after initialization.
*/
#test
{
    let a: u32 = 1 // Immutable variable
    @assert(a == 1)

    let b: string = "string"
    @assert(b == "string")

    var c: s32 = 42 // Mutable variable
    c += 1
    @assert(c == 43)
}

/**
# Multiple Variable Declarations

Swag allows declaring multiple variables of the same type on a single line.
*/
#test
{
    let a, b: u32 = 123
    @assert(a == 123)
    @assert(b == 123)
}

/**
Multiple variables of different types can also be declared on one line.
*/
#test
{
    let a: u32 = 12, b: f32 = 1.5
    @assert(a == 12)
    @assert(b == 1.5)
}

/**
# Default Initialization

If a variable is declared without an explicit value, it is automatically
initialized with a default value. Variables in Swag are **always initialized**
and never left undefined.
*/
#test
{
    var a: bool
    @assert(a == false)

    var b: string
    @assert(b == null)

    var c: f64
    @assert(c == 0)
}

/**
# Uninitialized Variables

To skip default initialization, you can assign 'undefined'. This prevents
automatic initialization but leaves the variable in an undefined state, which
should be used cautiously.
*/
#test
{
    var a: bool = undefined
    var b: string = undefined
}

/**
# Type Inference

Swag supports **type inference**, automatically determining a variable’s type
based on its assigned value. This often eliminates the need for explicit type
annotations.
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)
    #assert(#typeof(a) == f32)

    let b = "string"
    @assert(b == "string")
    #assert(#typeof(b) == string)

    let c = 1.5'f64
    @assert(c == 1.5)
    #assert(#typeof(c) == f64)
}

/**
Type inference also applies when declaring multiple variables simultaneously.
*/
#test
{
    let a, b = true
    @assert(a == true)
    @assert(b == true)
    #assert(#typeof(a) == #typeof(true))
    #assert(#typeof(b) == #typeof(a))

    let c = 1.5, d = "string"
    @assert(c == 1.5)
    @assert(d == "string")
    #assert(#typeof(c) == f32)
    #assert(#typeof(d) == string)
}

/**
# Special Variables

Swag provides attributes that modify how variables are stored or accessed.
*/

/**
## Thread-Local Storage

Global variables marked with '#[Swag.Tls]' are stored in thread-local storage.
Each thread has its own copy of the variable.
*/
#[Swag.Tls]
var G = 0 // Thread-local global variable

/**
## Global Variables

A local variable can be marked with '#[Swag.Global]' to make it global within
its function’s scope. It behaves similarly to 'static' variables in C/C++,
retaining its value between function calls.
*/
#test
{
    func toto()->s32
    {
        #[Swag.Global]
        var G1 = 0
        G1 += 1
        return G1
    }

    @assert(toto() == 1)
    @assert(toto() == 2)
    @assert(toto() == 3)
}

/**
## Compile-Time Variables

Global variables marked with '#[Swag.Compiler]' exist only during compile time
and are excluded from the final runtime code.
*/
#[Swag.Compiler]
var G2 = 0

#run
{
    G2 += 5 // Executes at compile time
}
