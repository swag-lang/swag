/**
# `with` Statement
The `with` statement is designed to reduce repetition by allowing you to access the fields and methods
of a variable or object within a specified scope. Inside a `with` block, you can use the `.` prefix to
refer to the fields or methods of the specified object, making the code more concise and easier to read.
*/

struct Point { x, y: s32 }

impl Point
{
    mtd setOne()
    {
        me.x, me.y = 1
        // Set both x and y to 1 within the Point instance.
    }
}

/**
# `with` on a Variable
The `with` statement can be used with a variable to streamline access to its fields and methods,
eliminating the need to repeatedly reference the variable name. This makes the code cleaner and reduces
clutter, especially when working with objects that have multiple fields or methods.
*/
#test
{
    var pt: Point // Declare a variable of type Point.
    with pt
    {
        .x = 1 // Equivalent to pt.x = 1, sets the x field.
        .y = 2 // Equivalent to pt.y = 2, sets the y field.
    }

    @assert(pt.x == 1) // Verify that x was set correctly.
    @assert(pt.y == 2) // Verify that y was set correctly.
}

/**
# `with` with Function Calls
The `with` statement also simplifies function calls on an object or struct by allowing direct
invocation of methods and access to fields within the `with` block. This approach helps maintain
cleaner and more intuitive code by reducing the repetition of the object or variable name.
*/
#test
{
    var pt: Point // Declare a variable of type Point.
    with pt
    {
        .setOne() // Equivalent to pt.setOne(), sets both x and y to 1.
        .y = 2 // Modify the y field directly within the block.
        @assert(.x == 1) // Equivalent to pt.x == 1, verifies that x is set to 1.
        @assert(.y == 2) // Equivalent to pt.y == 2, verifies that y is set to 2.
    }

    @assert(pt.x == 1) // Confirm that x remains correct after the with block.
    @assert(pt.y == 2) // Confirm that y remains correct after the with block.
}

/**
# `with` with a Namespace
The `with` statement can also be applied to a namespace, allowing you to call functions or access
constants within that namespace without needing to fully qualify the names. This is particularly useful
when working with large namespaces or when multiple calls to namespace members are required.
*/
#test
{
    with NameSpace
    {
        .inside0() // Equivalent to NameSpace.inside0(), calls the inside0 function.
        .inside1() // Equivalent to NameSpace.inside1(), calls the inside1 function.
    }
}

/**
# `with` with Variable Declaration
In addition to existing variables, the `with` statement can be used directly with variable declarations.
This allows you to immediately work with the fields of the newly declared variable within the scope of
the `with` block, streamlining initialization and setup tasks.
*/
#test
{
    with var pt = Point{1, 2} // Declare and initialize 'pt' with x=1, y=2.
    {
        .x = 10 // Modify x within the block.
        .y = 20 // Modify y within the block.
    }

    @assert(pt.x == 10 and pt.y == 20) // Ensure both fields are correctly updated.
}

#test
{
    with var pt: Point // Declare 'pt' without initialization.
    {
        .x = 10 // Set x to 10 within the block.
        .y = 20 // Set y to 20 within the block.
    }

    @assert(pt.x == 10 and pt.y == 20) // Ensure fields are set as expected.
}

/**
# `with` with an Assignment Statement
The `with` statement can also be used with an assignment, allowing you to immediately access and modify
the fields of the newly assigned value. This can be particularly helpful in scenarios where you want
to initialize or adjust an object’s fields immediately after creation or assignment.
*/
#test
{
    var pt: Point // Declare a variable of type Point.
    with pt = Point{1, 2} // Assign a new Point instance to 'pt'.
    {
        .x = 10 // Modify x within the block.
        .y = 20 // Modify y within the block.
    }

    @assert(pt.x == 10 and pt.y == 20) // Ensure fields are updated correctly.
}

namespace NameSpace
{
    func inside0()
    {
        // Define a function inside the namespace.
    }
    func inside1()
    {
        // Define another function inside the namespace.
    }
}
