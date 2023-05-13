/**
You can use `with` to avoid repeating the same variable again and again. You can then access fields with a simple `.`.
*/

struct Point { x, y: s32; }

impl Point
{
    mtd setOne()
    {
        x, y = 1
    }
}

#test
{
    // `with` on a variable.
    {
        var pt: Point
        with pt
        {
            .x = 1   // equivalent to pt.x
            .y = 2   // equivalent to pt.y
        }

        @assert(pt.x == 1)
        @assert(pt.y == 2)
    }

    // Works for function calls to.
    {
        var pt: Point
        with pt
        {
            .setOne()           // equivalent to pt.setOne() or setOne(pt)
            .y = 2              // equivalent to pt.y
            @assert(.x == 1)    // equivalent to pt.x
            @assert(.y == 2)    // equivalent to pt.y
            @assert(pt.x == 1)
            @assert(pt.y == 2)
        }
    }

    // Works also for enums.
    {
        enum RGB{R; G; B;}
        var color: RGB
        with color
        {
            x := .R
            @assert(x == RGB.R)
            @assert(x == .R)
        }
    }

    // Works also with a namespace.
    {
        with NameSpace
        {
            .inside0()
            .inside1()
        }
    }

    // Instead of an identifier name, `with` also accepts a variable declaration.
    {
        with pt := Point{1, 2}
        {
            .x = 10
            .y = 20
        }

        @assert(pt.x == 10 and pt.y == 20)
    }

    {
        with var pt: Point
        {
            .x = 10
            .y = 20
        }

        @assert(pt.x == 10 and pt.y == 20)
    }

    {
        enum RGB{R; G; B;}
        with var color: RGB
        {
            x := .R
            @assert(x == RGB.R)
            @assert(x == .R)
        }
    }

    // Or an affectation statement.
    {
        var pt: Point
        with pt = Point{1, 2}
        {
            .x = 10
            .y = 20
        }

        @assert(pt.x == 10 and pt.y == 20)
    }
}

namespace NameSpace
{
    func inside0() {}
    func inside1() {}
}