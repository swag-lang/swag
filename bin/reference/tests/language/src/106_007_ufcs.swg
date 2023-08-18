/**
*ufcs* stands for *uniform function call syntax*. It allows every functions to be called with a `param.func()` form when the first parameter of `func()` is of the same type as `param`.
*/

#test
{
    {
        func myFunc(param: bool) => param

        let b = false
        @assert(myFunc(b) == b.myFunc())
    }

    // This means that in Swag, there are only *static* functions, but which can be called like *methods*.
    {
        struct Point { x, y: s32; }
        func set(using pt: *Point, value: s32) { x, y = value; }

        var pt: Point

        // Ufcs call
        pt.set(10)
        @assert(pt.x == 10 and pt.y == 10)

        // Normal call
        set(&pt, 20)
        @assert(pt.x == 20 and pt.y == 20)
    }
}