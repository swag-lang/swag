#test
{
    // using brings the scope of a namespace, struct or enum in the current one
    {
        enum RGB { R; G; B; }
        @assert(RGB.R == 0)

        using RGB
        @assert(G == 1)
    }

    // using can be used with a variable or a function parameter
    {
        struct Point { x: s32; y: s32; }

        var pt: Point
        using pt

        x = 1
        y = 2
        @assert(pt.x == 1)
        @assert(pt.y == 2)
    }

    {
        struct Point { x: s32; y: s32; }

        func setOne(using point: *Point)
        {
            x, y = 1
        }

        var pt: Point
        setOne(&pt)
        @assert(pt.x == 1)
        @assert(pt.y == 1)
    }
}