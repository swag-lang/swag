/**
# 'using' with Enums and Namespaces
The 'using' statement can bring the scope of a namespace, struct, or enum into
the current context. This removes the need for full qualification when accessing
members. For enums, it simplifies code by avoiding repetitive type prefixes.
*/
#test
{
    enum RGB { R, G, B }
    @assert(RGB.R == 0) // Fully qualified access.

    using RGB // Import enum members into the current scope.
    @assert(G == 1) // 'G' accessible directly without 'RGB.' prefix.
}

/**
# 'using' with Struct Fields
The 'using' statement can be applied to struct fields, exposing the fields of
a nested struct as if they were part of the containing struct. This is useful
for composition-like behavior, improving code readability and reducing nesting.
*/
#test
{
    struct Point2 { x, y: s32 }

    struct Point3
    {
        using base:     Point2     // Bring 'Point2' fields into 'Point3' scope.
        z:              s32
    }

    var value: Point3

    // Fields from 'base' are directly accessible.
    value.x = 0
    value.y = 0
    value.z = 0

    @assert(&value.x == &value.base.x)
    @assert(&value.y == &value.base.y)

    // Automatic cast: Point3 can be treated as Point2.
    func set1(ptr: *Point2)
    {
        ptr.x, ptr.y = 1
    }

    set1(&value)

    @assert(value.x == 1)
    @assert(value.y == 1)
    @assert(value.base.x == 1)
    @assert(value.base.y == 1)
}
