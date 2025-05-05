/**
# Function Overloading with `Swag.Overload`
In Swag, it is possible to define multiple functions with the same name as long as their parameter signatures differ.
This capability, known as function overloading, allows you to provide different implementations of a function
depending on the number or types of arguments passed. To enable function overloading, the functions must be
decorated with the `Swag.Overload` attribute. This ensures that the compiler can correctly distinguish between
the different versions based on the arguments provided at the call site.
*/

#[Swag.ConstExpr, Swag.Overload]
{
    // Overloaded function with two parameters
    func sum(x, y: s32) => x + y                                // Sums two integers

    // Overloaded function with three parameters
    func sum(x, y, z: s32) => x + y + z                         // Sums three integers
}

/**
In the example above, the `sum` function is overloaded to handle both two-parameter and three-parameter cases.
When the function is called, the compiler automatically selects the appropriate version based on the number of arguments.
This allows for more flexible and intuitive code, where the same function name can be used for different operations
depending on the context.
*/

#assert(sum(1, 2) == 3)        // Calls the two-parameter version of `sum`, which returns 3
#assert(sum(1, 2, 3) == 6)     // Calls the three-parameter version of `sum`, which returns 6
