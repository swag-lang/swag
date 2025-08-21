/**
# Utilizing the `impl` Keyword with Enums in Swag

In Swag, the `impl` keyword allows you to define methods and functions within the scope of an enum.
This is particularly useful for associating specific behaviors directly with the enum values.
The `me` keyword within these methods refers to the current enum instance, providing a clean
and organized way to encapsulate logic related to the enum.
*/

// Define an enum RGB with three possible values
enum RGB
{
    R   // Represents the Red color
    G   // Represents the Green color
    B   // Represents the Blue color
}

/**
# Implementing Methods for an Enum

In the following example, we utilize the `impl enum` syntax to define methods for the `RGB` enum.
This syntax allows us to extend the functionality of the enum by associating methods with it.
The methods we define will operate on the enum's values, providing a clear and structured approach
to handling logic related to these values.
*/
impl enum RGB
{
    // Method to check if the current enum value is `R`.
    func isRed(me) => me == R

    // Method to check if the current enum value is either `R` or `B`.
    func isRedOrBlue(me) => me == R or me == B
}

#test
{
    // Testing the `RGB` enum methods

    // Verify if `RGB.R` is recognized as red
    @assert(RGB.isRed(RGB.R))

    // Verify if `RGB.B` is recognized as either red or blue
    @assert(RGB.isRedOrBlue(RGB.B))
}

/**
# Applying the `using` Keyword with Enum Methods

The `using` keyword simplifies method calls by allowing you to omit the enum type when invoking
methods. This results in more concise and readable code, especially when the enum type is used
frequently within a block.
*/
#test
{
    using RGB

    // Test the method without specifying the enum type
    @assert(isRedOrBlue(R))
    @assert(isRedOrBlue(B))
}

/**
# Introduction to Uniform Function Call Syntax (UFCS)

Uniform Function Call Syntax (UFCS) in Swag enhances code readability by allowing methods to be
called directly on enum values. This feature leads to more intuitive and clean code, particularly
when working with enums.
*/
#test
{
    using RGB

    @assert(R.isRedOrBlue())

    // Verify that `RGB.G` is not recognized as either red or blue
    @assert(!RGB.G.isRedOrBlue())
}
