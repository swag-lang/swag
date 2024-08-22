/**
# `using` with Enums and Namespaces
`using` brings the scope of a namespace, a struct, or an enum into the current one. This allows you to reference members of the enum, namespace, or struct without needing to fully qualify them.
*/
#test
{
    enum RGB { R, G, B }
    @assert(RGB.R == 0)

    using RGB
    @assert(G == 1)  // 'G' is directly accessible without 'RGB.'
}

/**
# `using` with Variables
`using` can also be applied to variables, allowing you to access the fields of a struct directly without needing to reference the variable name each time.
*/
#test
{
    struct Point { x: s32, y: s32 }

    var pt: Point

    using pt
    x = 1 // No need to specify 'pt' to access its fields
    y = 2 // No need to specify 'pt'

    @assert(pt.x == 1)
    @assert(pt.y == 2)
}

/**
# Declaring Variables with `using`
You can declare a variable using `using` directly, which automatically brings its fields into the current scope.
*/
#test
{
    struct Point { x: s32, y: s32 }
    using var pt: Point
    x = 1 // Direct access to fields without specifying 'pt'
    y = 2 // Direct access to fields without specifying 'pt'

    @assert(pt.x == 1)
    @assert(pt.y == 2)
}

/**
# `using` in Function Parameters
`using` applied to a function parameter can be seen as the equivalent of the hidden `this` pointer in C++. It allows you to refer to the fields of the passed struct directly, without needing to dereference the pointer or reference the parameter name.
*/
#test
{
    struct Point { x: s32, y: s32 }

    func setOne(using point: *Point)
    {
        // No need to specify 'point' to access its fields
        x, y = 1
    }

    var pt: Point
    setOne(&pt)  // 'using' makes 'x' and 'y' directly accessible inside 'setOne'
    @assert(pt.x == 1)
    @assert(pt.y == 1)

    // UFCS (Uniform Function Call Syntax)
    pt.setOne()  // You can call the function as if it were a method
    @assert(pt.x == 1)
    @assert(pt.y == 1)
}

/**
# `using` with Struct Fields
`using` can also be used with a field inside a struct. This allows the fields of a nested struct to be accessed directly as if they were part of the containing struct.
*/
#test
{
    struct Point2
    {
        x, y: s32
    }

    struct Point3
    {
        using base: Point2  // Brings 'Point2' fields into 'Point3'
        z: s32
    }

    // The content of 'base' can be referenced directly
    var value: Point3
    value.x = 0  // Equivalent to 'value.base.x = 0'
    value.y = 0  // Equivalent to 'value.base.y = 0'
    value.z = 0
    @assert(&value.x == &value.base.x)
    @assert(&value.y == &value.base.y)

    // The compiler can cast 'Point3' to 'Point2' automatically thanks to `using`
    func set1(using ptr: *Point2)
    {
        x, y = 1
    }

    set1(&value)  // The cast to 'Point2' is automatic
    @assert(value.x == 1)
    @assert(value.y == 1)
    @assert(value.base.x == 1)
    @assert(value.base.y == 1)
}
