/**
To declare a variable, use the `let` or `var` keyword, followed by `:` and then the type.

`let` is used for a variable that cannot be changed after the initial affectation.

`var` is used for a variable that can be changed again after the initial affectation.
*/
#test
{
    // Variable 'a' is of type 'u32' and its value is '1'. It cannot be changed.
    let a: u32 = 1
    @assert(a == 1)

    let b: string = "string"
    @assert(b == "string")

    // Variable 'c' is declared with 'var', so it can be changed.
    var c: s32 = 42
    c += 1
    @assert(c == 43)
}

/**
We have already seen that we can declare multiple variables on the same line.
*/
#test
{
    let a, b: u32 = 123
    @assert(a == 123)
    @assert(b == 123)
}

/**
Or
*/
#test
{
    let a: u32 = 12, b: f32 = 1.5
    @assert(a == 12)
    @assert(b == 1.5)
}

/**
If you don't assign a value, then the variable will be initialized with its default value (0). So a variable is **always** initialized.
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
But if you really do not want the variable to be initialized, you can assign it with `undefined`. To use with care, of course, but this is sometimes necessary to avoid the initialization cost.
*/
#test
{
    var a: bool = undefined
    var b: string = undefined
}

/**
We have seen that the type is optional in the declaration if it can be deduced from the assignment.
*/

/**
These are a bunch of **type inferences**.
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)
    #assert @typeof(a) == f32

    let b = "string"
    @assert(b == "string")
    #assert @typeof(b) == string

    let c = 1.5'f64
    @assert(c == 1.5)
    #assert @typeof(c) == f64
}

/**
The same goes for multiple variables.
*/
#test
{
    let a, b = true
    @assert(a == true)
    @assert(b == true)
    #assert @typeof(a) == @typeof(true)
    #assert @typeof(b) == @typeof(a)

    let c = 1.5, d = "string"
    @assert(c == 1.5)
    @assert(d == "string")
    #assert @typeof(c) == f32
    #assert @typeof(d) == string
}

/**
# Special variables
A global variable can be tagged with `#[Swag.Tls]` to store it in the thread local storage (one copy per thread).
*/
#[Swag.Tls]
var G = 0

/**
A local variable can be tagged with `#[Swag.Global]` to make it global (aka `static` in C/C++).
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
A global variable can also be marked as `#[Swag.Compiler]`. That kind of variable will not be exported to the runtime and can only be used in compile-time code.
*/
#[Swag.Compiler]
var G2 = 0

#run
{
    G2 += 5
}
