/**
# Introduction to Function Declarations

A function declaration starts with the 'func' keyword, followed by the function name
and parentheses. If no parameters are needed, the parentheses remain empty.
*/
#[Swag.Overload]
func toto() {}

/**
# Returning Values from Functions

If a function returns a value, use '->' followed by the return type.
The body must contain a 'return' statement.
*/
func toto1()->s32
{
    return 0
}

/**
# Inferring Return Types

Use '=>' for simple expressions when the return type can be inferred automatically.
*/
func sum(x, y: s32) => x + y

/**
# Defining Parameters in Functions

Parameters are declared within parentheses after the function name, each with a name and type.
*/
func sum1(x, y: s32, unused: f32)->s32
{
    return x + y
}

/**
# Using Default Parameter Values

Parameters may have default values, used when not provided by the caller.
*/
func sum2(x, y: s32, unused: f32 = 666)->s32
{
    return x + y
}

/**
# Inferred Parameter Types

If a parameter has a default value, its type can be inferred from it.
*/
func sum3(x, y = 0.0)
{
    #assert(#typeof(x) == f32)
    #assert(#typeof(y) == f32)
}

/**
# Overloading Functions

Multiple functions can share the same name if they differ in parameter count or types.
*/
enum Values { A, B }

#[Swag.Overload]
func toto(x: s32, y = Values.A)
{
    #assert(#typeof(y) == Values)
}

/**
# Nested Functions

Functions can be nested within other functions for local logic organization.
Nested functions are scoped, not closures.
*/
#test
{
    func sub(x, y: s32) => x - y

    let x = sub(5, 2)
    @assert(x == 3)
}

/**
# Named Parameters and Parameter Order

Named parameters allow calling functions with arguments in any order.
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
# Returning Multiple Values with Anonymous Structs

Functions can return anonymous structs to conveniently hold multiple values.
These can be accessed directly or destructured.
*/
#test
{
    func myFunction()->{ x, y: f32 }
    {
        return {1.0, 2.0}
    }

    let result = myFunction()
    @assert(result.item0 == 1.0)
    @assert(result.item1 == 2.0)

    let (x, y) = myFunction()
    @assert(x == 1.0)
    @assert(y == 2.0)

    let (z, w) = myFunction()
    @assert(z == 1.0)
    @assert(w == 2.0)
}
