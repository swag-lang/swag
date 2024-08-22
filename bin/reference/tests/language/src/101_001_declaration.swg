/**
# Introduction to Function Declarations
A function declaration typically begins with the `func` keyword followed by the function name.
*/
#[Swag.Overload]
func toto() {}

/**
# Returning Values from Functions
If the function needs to return a value, use `->` followed by the return type.
*/
func toto1() -> s32
{
    return 0
}

/**
# Inferring Return Types
For simple expressions, the return type can be inferred by using `=>` instead of `->`.
In this example, the return type is inferred as `s32`.
*/
func sum(x, y: s32) => x + y

/**
# Shorter Syntax for Functions Without Return Values
For functions that do not return a value, a shorter syntax can be used.
*/
func print(val: string) = @print(val)

/**
# Defining Parameters in Functions
Parameters are defined after the function name, within parentheses.
In this example, we declare two parameters, `x` and `y` of type `s32`, and an additional parameter of type `f32`.
*/
func sum1(x, y: s32, unused: f32) -> s32
{
    return x + y
}

/**
# Using Default Parameter Values
Parameters can have default values.
*/
func sum2(x, y: s32, unused: f32 = 666) -> s32
{
    return x + y
}

/**
# Inferred Parameter Types
If a parameter has a default value, its type can be inferred.
Here, `x` and `y` are inferred to be of type `f32` because `0.0` is a 32-bit floating-point literal.
*/
func sum3(x, y = 0.0)
{
    #assert @typeof(x) == f32
    #assert @typeof(y) == f32
}

/**
# Overloading Functions
Functions can be overloaded to handle different parameter types or counts.
*/
enum Values { A, B }

#[Swag.Overload]
func toto(x: s32, y = Values.A)
{
    #assert @typeof(y) == Values
}

/**
# Nested Functions
Functions can be nested inside other functions, providing a way to organize code within a specific scope.
*/
#test
{
    // These are not closures but functions within a sub-scope.
    func sub(x, y: s32) => x - y

    // Simple function call.
    let x = sub(5, 2)
    @assert(x == 3)
}

/**
# Named Parameters and Parameter Order
You can name parameters and call functions without respecting the parameter order.
*/
#test
{
    func sub(x, y: s32) => x - y

    {
        let x1 = sub(x: 5, y: 2)
        @assert(x1 == 3)
        let x2 = sub(y: 5, x: 2)
        @assert(x2 == -3)
    }

    {
        func returnMe(x, y: s32 = 0) => x + y * 2
        @assert(returnMe(x: 10) == 10)
        @assert(returnMe(y: 10) == 20)
    }
}

/**
# Multiple Return Values
Functions in Swag can return multiple values, often using an anonymous struct or tuple.
*/

/**
## Returning Multiple Values with Anonymous Structs
An **anonymous struct** can be used to return multiple values from a function.
*/
#test
{
    func myFunction() -> { x, y: f32 }
    {
        return {1.0, 2.0}
    }

    var result = myFunction()
    @assert(result.item0 == 1.0)
    @assert(result.item1 == 2.0)

    let (x, y) = myFunction()
    @assert(x == 1.0)
    @assert(y == 2.0)

    let (z, w) = myFunction()
    @assert(z == 1.0)
    @assert(w == 2.0)
}

/**
## Using `retval` to Structure Return Statements
You can also use the `retval` type, which is an alias for the function's return type, to structure your return statements.
This allows for more flexibility in complex return scenarios.
*/
#test
{
    func returns2() -> { x: s32, y: s32 }
    {
        // You can return a tuple literal as long as the types match.
        if false:
            return {1, 2}

        // Alternatively, use the special type 'retval' as a type alias to the function's return type.
        using var result: retval
        x = 1
        y = 2
        return result
    }

    // You can access the struct fields by name or using 'item?'.
    var result = returns2()
    @assert(result.x == 1)
    @assert(result.item0 == 1)
    @assert(result.y == 2)
    @assert(result.item1 == 2)

    // You can also deconstruct the returned struct.
    let (x, y) = returns2()
    @assert(x == 1)
    @assert(y == 2)
}
