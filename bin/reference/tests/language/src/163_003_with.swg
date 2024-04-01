/**
You can use `with` to avoid repeating the same variable again and again. You can then access fields with a simple `.`.
*/

struct Point { x, y: s32 }

impl Point
{
    mtd setOne()
    {
        x, y = 1
    }
}

/**
`with` on a variable.
*/
#test
{
    var pt: Point
    with pt
    {
        .x = 1 // equivalent to pt.x
        .y = 2 // equivalent to pt.y
    }

    @assert(pt.x == 1)
    @assert(pt.y == 2)
}

/**
Works for function calls to.
*/
#test
{
    var pt: Point
    with pt
    {
        .setOne() // equivalent to pt.setOne() or setOne(pt)
        .y = 2 // equivalent to pt.y
        @assert(.x == 1) // equivalent to pt.x
        @assert(.y == 2) // equivalent to pt.y
        @assert(pt.x == 1)
        @assert(pt.y == 2)
    }
}

/**
Works also with a namespace.
*/
#test
{
    with NameSpace
    {
        .inside0()
        .inside1()
    }
}

/**
Instead of an identifier name, `with` also accepts a variable declaration.
*/
#test
{
    with var pt = Point{1, 2}
    {
        .x = 10
        .y = 20
    }

    @assert(pt.x == 10 and pt.y == 20)
}

#test
{
    with var pt: Point
    {
        .x = 10
        .y = 20
    }

    @assert(pt.x == 10 and pt.y == 20)
}

/**
Or an affectation statement.
*/
#test
{
    var pt: Point
    with pt = Point{1, 2}
    {
        .x = 10
        .y = 20
    }

    @assert(pt.x == 10 and pt.y == 20)
}

namespace NameSpace
{
    func inside0() {}
    func inside1() {}
}
