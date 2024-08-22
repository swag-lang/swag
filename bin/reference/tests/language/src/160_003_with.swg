/**
# `with` Statement
You can use `with` to avoid repeating the same variable again and again. Within the `with` block, you can access fields with a simple `.` prefix, which refers to the fields or methods of the variable or object specified in the `with`.
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
# `with` on a Variable
`with` can be used with a variable to streamline access to its fields and methods.
*/
#test
{
    var pt: Point
    with pt
    {
        .x = 1 // Equivalent to pt.x
        .y = 2 // Equivalent to pt.y
    }

    @assert(pt.x == 1)
    @assert(pt.y == 2)
}

/**
# `with` with Function Calls
The `with` statement also simplifies function calls on the object or struct. You can call methods or access fields directly within the `with` block.
*/
#test
{
    var pt: Point
    with pt
    {
        .setOne() // Equivalent to pt.setOne() or setOne(pt)
        .y = 2 // Equivalent to pt.y
        @assert(.x == 1) // Equivalent to pt.x
        @assert(.y == 2) // Equivalent to pt.y
        @assert(pt.x == 1)
        @assert(pt.y == 2)
    }
}

/**
# `with` with a Namespace
`with` can also be used with a namespace, allowing you to call functions or access constants within that namespace without repeating the namespace name.
*/
#test
{
    with NameSpace
    {
        .inside0() // Equivalent to NameSpace.inside0()
        .inside1() // Equivalent to NameSpace.inside1()
    }
}

/**
# `with` with Variable Declaration
Instead of an existing variable, `with` can also be used directly with a variable declaration.
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
# `with` with an Assignment Statement
`with` can also be used with an assignment statement, allowing you to immediately access and modify the fields of the assigned value.
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
