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
    func set1(ptr: *Point2)
    {
        ptr.x, ptr.y = 1 // Direct access to 'x' and 'y' fields.
    }

    set1(&value)          // Automatic cast to 'Point2' and modify 'value'.
    @assert(value.x == 1) // Confirm 'x' was correctly set.
    @assert(value.y == 1) // Confirm 'y' was correctly set.
    @assert(value.base.x == 1)  // Ensure 'base.x' was updated.
    @assert(value.base.y == 1)  // Ensure 'base.y' was updated.
}
