/**
# `using` with Enums and Namespaces
The `using` statement allows you to bring the scope of a namespace, struct, or enum into the current 
scope. This makes it possible to reference members directly without the need for full qualification. 
For example, when working with enums, this can simplify the code by removing the need to constantly 
prefix enum values with the enum type name.
*/
#test
{
    enum RGB { R, G, B }
    @assert(RGB.R == 0)   // Accessing the enum member with full qualification.

    using RGB             // Bringing the enum members into the current scope.
    @assert(G == 1)       // 'G' is now directly accessible without the 'RGB.' prefix.
}

/**
# `using` with Variables
The `using` statement can also be applied to variables, particularly those of struct types. This allows 
you to access the fields of a struct directly within the current scope, eliminating the need to 
reference the variable name each time you access a field. This is particularly useful for reducing 
code verbosity and improving readability.
*/
#test
{
    struct Point { x: s32, y: s32 }

    var pt: Point         // Declaring a variable of struct type 'Point.'

    using pt              // Bringing the fields of 'pt' into the current scope.
    x = 1                 // Direct access to 'x' without needing 'pt.x'.
    y = 2                 // Direct access to 'y' without needing 'pt.y'.

    @assert(pt.x == 1)    // Verifying that 'x' was set correctly.
    @assert(pt.y == 2)    // Verifying that 'y' was set correctly.
}

/**
# Declaring Variables with `using`
You can declare a variable with the `using` keyword, which immediately brings the variable’s fields 
into the current scope. This approach can streamline your code by allowing direct access to struct 
fields without the need to prefix them with the variable name, making the code cleaner and more 
concise.
*/
#test
{
    struct Point { x: s32, y: s32 }
    using var pt: Point   // Declare 'pt' and bring its fields into the current scope.
    x = 1                 // Direct access to 'x' without specifying 'pt'.
    y = 2                 // Direct access to 'y' without specifying 'pt'.

    @assert(pt.x == 1)    // Ensure that 'x' was correctly set.
    @assert(pt.y == 2)    // Ensure that 'y' was correctly set.
}

/**
# `using` in Function Parameters
When applied to function parameters, `using` allows fields of a struct to be accessed directly within 
the function, similar to how a `this` pointer works in C++. This can simplify function code by 
eliminating the need to repeatedly dereference a pointer or reference a parameter name, making the 
function logic clearer and easier to follow.
*/
#test
{
    struct Point { x: s32, y: s32 }

    func setOne(using point: *Point)
    {
        // Access the fields of 'point' directly without prefixing with 'point->'.
        x, y = 1
    }

    var pt: Point
    setOne(&pt)           // Call 'setOne' and modify 'pt' directly.
    @assert(pt.x == 1)    // Validate that 'x' was set correctly.
    @assert(pt.y == 1)    // Validate that 'y' was set correctly.

    // UFCS (Uniform Function Call Syntax) allows calling the function as if it were a method.
    pt.setOne()           // Equivalent to 'setOne(&pt)'.
    @assert(pt.x == 1)    // Ensure 'x' remains correct.
    @assert(pt.y == 1)    // Ensure 'y' remains correct.
}

/**
# `using` with Struct Fields
The `using` statement can also be applied to a field within a struct. This allows the fields of a 
nested struct to be accessed as if they were part of the containing struct. This feature is especially 
useful when working with inheritance or composition, enabling cleaner and more intuitive code by 
removing unnecessary layers of field access.
*/
#test
{
    struct Point2
    {
        x, y: s32        // Define two fields, 'x' and 'y'.
    }

    struct Point3
    {
        using base: Point2  // Bring 'Point2' fields into 'Point3' scope.
        z: s32              // Define an additional field 'z'.
    }

    // The 'base' fields can now be referenced directly through 'Point3'.
    var value: Point3
    value.x = 0           // Direct access to 'x', equivalent to 'value.base.x = 0'.
    value.y = 0           // Direct access to 'y', equivalent to 'value.base.y = 0'.
    value.z = 0           // Access 'z' directly, as it is part of 'Point3'.
    @assert(&value.x == &value.base.x)  // Validate that 'x' refers to the correct memory location.
    @assert(&value.y == &value.base.y)  // Validate that 'y' refers to the correct memory location.

    // The compiler can automatically cast 'Point3' to 'Point2' due to the `using` statement.
    func set1(using ptr: *Point2)
    {
        x, y = 1          // Direct access to 'x' and 'y' fields.
    }

    set1(&value)          // Automatic cast to 'Point2' and modify 'value'.
    @assert(value.x == 1) // Confirm 'x' was correctly set.
    @assert(value.y == 1) // Confirm 'y' was correctly set.
    @assert(value.base.x == 1)  // Ensure 'base.x' was updated.
    @assert(value.base.y == 1)  // Ensure 'base.y' was updated.
}
