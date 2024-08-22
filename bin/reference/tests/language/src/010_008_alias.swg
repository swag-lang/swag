/**
# Type Alias
A `typealias` is used to create a shortcut to another type, allowing you to refer to that type with a new, possibly more descriptive name.
*/

/**
# Basic Type Alias
`typealias` is used to make a shortcut to another type. This allows you to use the alias in place of the original type, simplifying code or improving readability.
*/
#test
{
    enum RGB { R, G, B }
    @assert(RGB.R == 0)

    typealias Color = RGB // 'Color' is now equivalent to 'RGB'
    @assert(Color.G == 1) // You can use 'Color' wherever 'RGB' is expected.
}

/**
You can then use the new name in place of the original type. This does not create a new type but rather an alias for the existing type.
*/
#test
{
    typealias Float32 = f32
    typealias Float64 = f64

    var x: Float32 = 1.0     // Same as 'f32'
    var y: Float64 = 1.0     // Same as 'f64'
    #assert @typeof(Float32) == f32
    #assert @typeof(Float64) == f64
}

/**
# Strict Type Alias
To create a new type that enforces type safety, a `typealias` can be marked with the `Swag.Strict` attribute. This prevents implicit casting to or from the aliased type, making it a distinct type, but explicit casts are still possible.
*/
#test
{
    #[Swag.Strict]
    typealias MyType = s32
    #assert @typeof(MyType) != s32

    let x: MyType = cast(MyType) 0 // Explicit cast is required.
    let y: s32 = cast(s32) x       // Casting back to 's32' also requires an explicit cast.
}

/**
# Name Alias
A `namealias` is used to create a shortcut or an alternative name for functions, variables, or namespaces, simplifying code and making it easier to manage long names.
*/

/**
# Function Name Alias
You can alias a function name with `namealias`, allowing you to call a function with a shorter or more convenient name.
*/
#test
{
    func thisIsABigFunctionName(x: s32) => x * x

    namealias myFunc = thisIsABigFunctionName
    @assert(myFunc(4) == 16) // 'myFunc' is now an alias for 'thisIsABigFunctionName'.
}

/**
# Variable and Namespace Alias
You can also alias variables and namespaces, providing a shorter name that can be used throughout the code.
*/
#test
{
    var myLongVariableName: s32 = 0
    namealias short = myLongVariableName

    short += 2  // 'short' is an alias for 'myLongVariableName'.
    @assert(myLongVariableName == 2) // The original variable reflects the changes made through the alias.
}
