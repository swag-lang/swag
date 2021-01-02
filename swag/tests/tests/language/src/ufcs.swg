#test
{
    // ufcs stands for "universal function call system"
    // it allows every functions to be called with a 'param.func()' form when the
    // first parameter of func() is of the same type as 'param'
    {
        func myFunc(param: bool) => param

        b := false
        @assert(myFunc(b) == b.myFunc())
    }

    // It means that in swag, there are only 'static' functions, but that
    // can be called like 'methods'
    {
        struct Point { x, y: s32; }
        func set(using pt: *Point, value: s32) = x, y = value

        var pt: Point
        pt.set(10)
        @assert(pt.x == 10 && pt.y == 10)

        set(&pt, 20)
        @assert(pt.x == 20 && pt.y == 20)
    }
}