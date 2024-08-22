/**
# Using `impl` with Enums in Swag

The `impl` keyword in Swag allows you to define methods and functions within the scope of an enum. This feature enhances the functionality of enums by enabling you to associate behaviors directly with the enum values. The keyword `self` is used within these methods to refer to the current enum value.
*/

// Define an enum RGB with three values
enum RGB
{
    R
    G
    B
}

/**
# Defining Methods with `impl`

In the example below, we use the `impl enum` syntax to define methods for the `RGB` enum. The `impl` keyword is used to declare that the following block contains methods associated with the `RGB` enum. We will explore `impl` further when we look at structs, but the concept is similar.
*/
impl enum RGB
{
    // This method checks if the enum value is `R`.
    func isRed(self) => self == R

    // This method checks if the enum value is either `R` or `B`.
    func isRedOrBlue(self) => self == R or self == B
}

#test
{
    // Test the methods defined in the `impl` block.

    // Check if `RGB.R` is red
    @assert(RGB.isRed(RGB.R))

    // Check if `RGB.B` is red or blue
    @assert(RGB.isRedOrBlue(RGB.B))

    /**
    # Using `using` with Enum Methods

    The `using` keyword allows us to omit the enum type when calling methods, making the code cleaner and more concise.
    */
    using RGB
    @assert(isRedOrBlue(R))
    @assert(isRedOrBlue(B))

    /**
    # Uniform Function Call Syntax (UFCS)

    A first look at Uniform Function Call Syntax (UFCS): UFCS allows calling methods directly on enum values, improving readability and making the code more intuitive.
    */
    @assert(R.isRedOrBlue())
    @assert(!RGB.G.isRedOrBlue())
}
