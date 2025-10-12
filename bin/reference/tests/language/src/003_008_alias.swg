/**
# Type Alias

An 'alias' creates a shorthand or alternative name for an existing type. This can
improve readability, reduce repetition, and simplify the use of complex types.
A type alias does not create a new type — it simply provides a new name for an
existing one.
*/

/**
# Basic Type Alias

Using 'alias', you can define a shorthand for any existing type. The alias can be
used interchangeably with the original type throughout your code.
*/
#test
{
    enum RGB { R, G, B }
    @assert(RGB.R == 0)

    alias Color = RGB
    @assert(Color.G == 1)
}

/**
# Aliasing Primitive Types

Aliases can be created for primitive types to make your code more expressive or
domain-specific, improving clarity when a type has a specific contextual meaning.
*/
#test
{
    alias Float32 = f32
    alias Float64 = f64

    var x: Float32 = 1.0
    var y: Float64 = 1.0

    #assert(#typeof(Float32) == f32)
    #assert(#typeof(Float64) == f64)
}

/**
# Strict Type Alias

When you need stronger type safety, you can use the '#[Swag.Strict]' attribute.
This makes the alias a **distinct type**, disallowing implicit conversions between
the alias and its base type. Explicit casting is still permitted.
*/
#test
{
    #[Swag.Strict]
    alias MyType = s32
    #assert(#typeof(MyType) != s32)

    let x: MyType = cast(MyType) 0
    let y: s32 = cast(s32) x
}

/**
# Name Alias

The 'alias' keyword can also create shortcuts for functions, variables, or
namespaces. This helps simplify long names and improve code readability.
*/

/**
## Function Name Alias

A function can be aliased to a shorter or more convenient name, which is
especially useful for lengthy or descriptive function names.
*/
#test
{
    func thisIsABigFunctionName(x: s32) => x * x

    alias myFunc = thisIsABigFunctionName
    @assert(myFunc(4) == 16)
}

/**
## Variable and Namespace Alias

You can alias variables or namespaces to shorter names for easier access,
especially in large codebases with long identifiers.
*/
#test
{
    var myLongVariableName: s32 = 0
    alias short = myLongVariableName

    short += 2
    @assert(myLongVariableName == 2)
}
