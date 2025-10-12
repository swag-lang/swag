/**
# Function Overloading with `Swag.Overload`
Swag allows multiple functions to share the same name when their parameter signatures differ.
This feature, called *function overloading*, enables writing concise and intuitive APIs.
To activate it, each version of the function must be decorated with `#[Swag.Overload]`.
*/

#[Swag.ConstExpr, Swag.Overload]
{
    // Overload: two parameters
    func sum(x, y: s32) => x + y

    // Overload: three parameters
    func sum(x, y, z: s32) => x + y + z
}

/**
The compiler chooses the correct overload based on the number and types of arguments.
This allows calling `sum` naturally for different scenarios.
*/

#assert(sum(1, 2) == 3) // Calls the two-parameter version
#assert(sum(1, 2, 3) == 6) // Calls the three-parameter version
