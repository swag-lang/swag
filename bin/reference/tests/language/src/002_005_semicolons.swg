/**
# Statement Termination in Swag

Unlike languages such as C or C++, there is no strict requirement to end statements
with a semicolon (';'). In Swag, a statement naturally terminates at the end of a line
('end of line'). This allows for cleaner, more concise syntax and reduces visual clutter,
making the code easier to read and maintain.
*/

#test
{
    // Declare two variables, 'x' and 'y', both of type 's32' (signed 32-bit integer),
    // and initialize them with the value '1'.
    var x, y: s32 = 1

    // Increment the value of both 'x' and 'y' by 1.
    x += 1
    y += 1

    // Use the '@assert' intrinsic to verify the correctness of the logic.
    @assert(x == 2) // Confirms that 'x' equals 2. Raises an error if it fails.
    @assert(y == x) // Confirms that 'y' equals 'x', which should also be 2.
}

/**
# Optional Semicolons

While semicolons are optional, they can still be used if desired. In some cases,
using semicolons may enhance code clarity or readability, particularly in more
complex statements or when writing multiple statements on a single line.
*/

#test
{
    // Here, type inference is used, so the types of 'x' and 'y' are not explicitly defined.
    var x, y = 0
    // Both 'x' and 'y' are initialized to 0, with types inferred automatically.

    // Increment both variables by 1.
    x, y += 1
    // Both 'x' and 'y' now have the value 1.
}

/**
# Multiple Statements on a Single Line

Semicolons are especially useful when writing multiple statements on a single line.
Although this can make code more compact, it is generally recommended to use it
sparingly, as it may reduce readability in complex code.
*/

#test
{
    // Two variable declarations and initializations on a single line.
    var x = 0
    var y = 0

    // Increment both 'x' and 'y' on the same line.
    x += 1; y += 1

    // Assert correctness of both variables.
    @assert(x == 1)
    @assert(y == 1)
}
