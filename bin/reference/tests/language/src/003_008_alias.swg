/**
# Type Alias
An `alias` allows you to create a shorthand for an existing type, making it possible to refer to that type using a new, potentially more descriptive name. This can enhance code clarity, reduce repetition, and make complex types easier to work with.
*/

/**
# Basic Type Alias
Using `alias`, you can define an alias for an existing type. This alias can then be used in place of the original type, simplifying the code or improving its readability. It’s important to note that a type alias does not create a new type but merely provides an alternative name for an existing one.
*/
#test
{
    enum RGB { R, G, B }
    @assert(RGB.R == 0)

    alias Color = RGB                   // 'Color' is now a shorthand for 'RGB'
    @assert(Color.G == 1)                   // 'Color' can be used wherever 'RGB' is expected.
}

/**
# Aliasing Primitive Types
You can create aliases for primitive types, making your code more readable and meaningful, especially when the alias represents a specific domain concept.
*/
#test
{
    alias Float32 = f32                 // 'Float32' is an alias for 'f32'
    alias Float64 = f64                 // 'Float64' is an alias for 'f64'

    var x: Float32 = 1.0                    // Equivalent to declaring 'var x: f32'
    var y: Float64 = 1.0                    // Equivalent to declaring 'var y: f64'
    #assert #typeof(Float32) == f32
    #assert #typeof(Float64) == f64
}

/**
# Strict Type Alias
In cases where you need to enforce type safety, Swag provides the `Swag.Strict` attribute, which can be applied to a `alias`. This makes the alias a distinct type, preventing implicit casting between the alias and the original type. Explicit casting is still allowed, but the distinct type ensures that operations requiring type specificity are clear and deliberate.
*/
#test
{
    #[Swag.Strict]
    alias MyType = s32                  // 'MyType' is a distinct type, not interchangeable with 's32'
    #assert #typeof(MyType) != s32

    let x: MyType = cast(MyType) 0          // Explicit cast is required to assign a value
    let y: s32 = cast(s32) x                // Casting back to 's32' also requires an explicit cast
}

/**
# Name Alias
An `alias` allows you also to create an alternative name or shortcut for functions, variables, or namespaces. This can be particularly useful for simplifying code, managing long or complex names, or improving the readability and maintainability of your code.
*/

/**
# Function Name Alias
With `alias`, you can define a shorter or more convenient name for a function. This is particularly helpful when dealing with functions that have long or descriptive names, allowing you to simplify your code without sacrificing functionality.
*/
#test
{
    func thisIsABigFunctionName(x: s32) => x * x

    alias myFunc = thisIsABigFunctionName
    @assert(myFunc(4) == 16)                // 'myFunc' is now an alias for 'thisIsABigFunctionName'
}

/**
# Variable and Namespace Alias
`alias` can also be used to alias variables and namespaces, offering a shorter or more convenient reference that can be used throughout your code. This is particularly useful in large codebases where certain variables or namespaces are frequently referenced.
*/
#test
{
    var myLongVariableName: s32 = 0
    alias short = myLongVariableName

    short += 2                              // 'short' is an alias for 'myLongVariableName'
    @assert(myLongVariableName == 2)        // The original variable reflects changes made via the alias
}
