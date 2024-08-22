/**
# Function Overloading with `Swag.Overload`
In Swag, functions can have the same name as long as their parameters differ. This is known as function overloading. To enable this feature, the functions must be marked with the `Swag.Overload` attribute.
*/

#[Swag.ConstExpr, Swag.Overload]
{
    // Overloaded function with two parameters
    func sum(x, y: s32) => x + y

    // Overloaded function with three parameters
    func sum(x, y, z: s32) => x + y + z
}

/**
# Example: Using Overloaded Functions
In the example below, the `sum` function is overloaded to handle both two-parameter and three-parameter versions. The compiler determines which version to call based on the number of arguments provided.
*/

#assert sum(1, 2) == 3      // Calls the two-parameter version of `sum`
#assert sum(1, 2, 3) == 6   // Calls the three-parameter version of `sum`
