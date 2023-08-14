#test
{
    // `using` brings the scope of a namespace, a struct or an enum in the current one.
    {
        enum RGB { R; G; B; }
        @assert(RGB.R == 0)

        using RGB
        @assert(G == 1)
    }

    // `using` can also be used with a variable
    {
        struct Point { x: s32, y: s32 }

        var pt: Point

        using pt
        x = 1 // no need to specify 'pt'
        y = 2 // no need to specify 'pt'

        @assert(pt.x == 1)
        @assert(pt.y == 2)
    }

    // You can declare a variable with `using` just before.
    {
        struct Point { x: s32; y: s32; }

        using var pt: Point
        x = 1 // no need to specify 'pt'
        y = 2 // no need to specify 'pt'

        @assert(pt.x == 1)
        @assert(pt.y == 2)
    }
}

/**
# For a function parameter
`using` applied to a function parameter can be seen as the equivalent of the hidden `this` in C++.
*/
#test
{
    struct Point { x: s32; y: s32; }

    func setOne(using point: *Point)
    {
        // Here there's no need to specify 'point'
        x, y = 1
    }

    var pt: Point
    setOne(&pt)
    @assert(pt.x == 1)
    @assert(pt.y == 1)

    // ufcs
    pt.setOne()
    @assert(pt.x == 1)
    @assert(pt.y == 1)
}

/**
# For a field
`using` can also be used with a field inside a struct.
*/
#test
{
    struct Point2
    {
        x, y: s32
    }

    struct Point3
    {
        using base: Point2
        z: s32
    }

    // That way the content of the field can be referenced directly
    var value: Point3
    value.x = 0 // Equivalent to value.base.x = 0
    value.y = 0 // Equivalent to value.base.y = 0
    value.z = 0
    @assert(&value.x == &value.base.x)
    @assert(&value.y == &value.base.y)

    // The compiler can then cast automatically 'Point3' to 'Point2'
    func set1(using ptr: *Point2)
    {
        x, y = 1
    }

    set1(&value) // Here the cast is automatic thanks to the using
    @assert(value.x == 1)
    @assert(value.y == 1)
    @assert(value.base.x == 1)
    @assert(value.base.y == 1)
}