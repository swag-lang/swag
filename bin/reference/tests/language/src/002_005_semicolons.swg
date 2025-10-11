/**
# Statement Termination in Swag

Unlike languages such as C/C++, there is no strict requirement to end statements with a semicolon (`;`). The default method of terminating a statement in Swag is simply reaching the end of a line (`end of line`). This feature allows for cleaner and more concise syntax, reducing visual clutter and making the code easier to read.
*/

#test
{
    // Declare two variables, x and y, both of type s32 (signed 32-bit integer),
    // and initialize them with the value 1.
    var x, y: s32 = 1

    // Increment the value of both x and y by 1.
    x += 1
    y += 1

    // Use the '@assert' intrinsic to verify the correctness of the program logic.
    @assert(x == 2) // Confirms that x equals 2. An error is raised if this assertion fails.
    @assert(y == x) // Confirms that y equals x, which should also be 2 at this point.
}

/**
# Optional Semicolons

While semicolons are optional, they can still be used if desired. In some cases, using semicolons might enhance the clarity of the code or improve readability, particularly when dealing with more complex statements or when writing multiple statements on a single line.
*/

#test
{
    // Here, type inference is used, so the types of x and y are not explicitly defined.
    var x, y = 0
    // Both x and y are initialized to 0, with their types inferred automatically.

    // The following statement increments both x and y by 1 simultaneously.
    x, y += 1
    // Both x and y now have the value 1.
}

/**
# Multiple Statements on a Single Line

Semicolons become particularly useful when you need to write multiple statements on a single line.
Although this can make the code more compact, it is advisable to use this approach sparingly,
as excessive use may negatively impact code readability.
*/

#test
{
    // Two variable declarations and initializations on a single line.
    var x = 0
    var y = 0

    // Increment both x and y by 1, all on the same line.
    x += 1; y += 1

    // Assert the correctness of both x and y values on the same line.
    @assert(x == 1)
    @assert(y == 1)
}
