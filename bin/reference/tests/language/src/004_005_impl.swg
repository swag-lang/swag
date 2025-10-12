/**
# Implementing Methods for Enums in Swag

Swag allows the use of the `impl` keyword with enums to define methods directly associated with them.
This makes it possible to attach behavior to enum values, enhancing code organization and encapsulation.

Within these methods, the `me` keyword refers to the current enum instance, similar to `this` in other languages.
*/

enum RGB
{
    R     // Represents Red
    G     // Represents Green
    B     // Represents Blue
}

/**
# Defining Enum Methods with `impl enum`

The `impl enum` block defines methods that operate on enum values.
This helps group functionality logically with the type it belongs to.
*/
impl enum RGB
{
    // Check if the current color is Red
    func isRed(me) => me == R

    // Check if the current color is Red or Blue
    func isRedOrBlue(me) => me == R or me == B
}

#test
{
    // Verify if `RGB.R` is recognized as red
    @assert(RGB.isRed(RGB.R))

    // Verify if `RGB.B` is recognized as either red or blue
    @assert(RGB.isRedOrBlue(RGB.B))
}

/**
# Simplifying Calls with `using`

The `using` keyword allows you to call enum methods without qualifying them with the enum name.
This makes the code cleaner when the enum type is already in scope.
*/
#test
{
    using RGB

    @assert(isRedOrBlue(R))
    @assert(isRedOrBlue(B))
}

/**
# Uniform Function Call Syntax (UFCS)

Swag supports *Uniform Function Call Syntax (UFCS)*, meaning methods can be called directly on enum values.
This provides a natural, object-oriented style of method invocation.
*/
#test
{
    using RGB

    // Call `isRedOrBlue` directly on the enum value
    @assert(R.isRedOrBlue())

    // Verify that `G` is not recognized as red or blue
    @assert(!G.isRedOrBlue())
}
