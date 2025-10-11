/**
# Introduction to Function Declarations
A function declaration typically starts with the `func` keyword, followed by the function name and
a pair of parentheses. If no parameters are needed, the parentheses are empty.
*/
#[Swag.Overload]
func toto() {}

/**
# Returning Values from Functions
If a function is intended to return a value, use `->` followed by the return type. The function body
must include a `return` statement with the corresponding value.
*/
func toto1()->s32
{
    return 0 // Returns an integer value of 0
}

/**
# Inferring Return Types
For simple expressions, the return type can be inferred automatically using the `=>` operator
instead of explicitly declaring it with `->`. This reduces verbosity in cases where the type
is obvious.
*/
func sum(x, y: s32) => x + y // Return type is inferred as s32

/**
# Shorter Syntax for Functions Without Return Values
When a function does not return a value, a concise syntax can be used also. Instead of a full function
body, a single expression can be provided after `=>`.
*/
func print(val: string) => @print(val) // Prints the given string value

/**
# Defining Parameters in Functions
Function parameters are defined within parentheses following the function name. Each parameter is
declared with a name and type, separated by a colon. In this example, two parameters `x` and `y`
of type `s32`, and an additional `unused` parameter of type `f32` are defined.
*/
func sum1(x, y: s32, unused: f32)->s32
{
    return x + y // Returns the sum of x and y
}

/**
# Using Default Parameter Values
Parameters can be assigned default values. If the caller does not provide a value for such a
parameter, the default value is used.
*/
func sum2(x, y: s32, unused: f32 = 666)->s32
{
    return x + y // Returns the sum of x and y
}

/**
# Inferred Parameter Types
If a parameter has a default value, its type can be inferred from the value provided.
In this example, `x` and `y` are inferred as `f32` due to the `0.0` literal.
*/
func sum3(x, y = 0.0)
{
    #assert(#typeof(x) == f32)
    // Asserts that x is of type f32
    #assert(#typeof(y) == f32)
    // Asserts that y is of type f32
}

/**
# Overloading Functions
Swag allows function overloading, where multiple functions can share the same name but differ
in their parameter types or counts.
*/
enum Values { A, B }

#[Swag.Overload]
func toto(x: s32, y = Values.A)
{
    #assert(#typeof(y) == Values)
    // Asserts that y is of type Values
}

/**
# Nested Functions
Functions can be nested within other functions to provide encapsulation and organize code.
These nested functions are not closures but are limited to the scope in which they are declared.
*/
#test
{
    func sub(x, y: s32) => x - y // Defines a nested function

    let x = sub(5, 2) // Calls the nested function
    @assert(x == 3) // Asserts the result of the function call
}

/**
# Named Parameters and Parameter Order
Swag supports named parameters, allowing you to specify arguments in any order when calling
a function. This can enhance code readability and flexibility.
*/
#test
{
    func sub(x, y: s32) => x - y // Defines a function for subtraction
    {
        let x1 = sub(x: 5, y: 2) // Calls with named parameters
        @assert(x1 == 3) // Asserts that the result is correct
        let x2 = sub(y: 5, x: 2) // Calls with parameters in reversed order
        @assert(x2 == -3) // Asserts that the result is correct
    }

    {
        func returnMe(x, y: s32 = 0) => x + y * 2 // Defines a function with a default parameter
        @assert(returnMe(x: 10) == 10) // Calls with one parameter
        @assert(returnMe(y: 10) == 20) // Calls with the second parameter only
    }
}

/**
# Returning Multiple Values with Anonymous Structs
Anonymous structs provide a convenient way to return multiple values from a function.
This method facilitates accessing returned data either by destructuring or directly from
the struct.
*/
#test
{
    func myFunction()->{ x, y: f32 }
    {
        return {1.0, 2.0}
    }

    let result = myFunction() // Stores the result in a variable
    @assert(result.item0 == 1.0) // Asserts the first item
    @assert(result.item1 == 2.0) // Asserts the second item

    let (x, y) = myFunction() // Destructures the result into variables
    @assert(x == 1.0) // Asserts the value of x
    @assert(y == 2.0) // Asserts the value of y

    let (z, w) = myFunction() // Another destructuring example
    @assert(z == 1.0) // Asserts the value of z
    @assert(w == 2.0) // Asserts the value of w
}
