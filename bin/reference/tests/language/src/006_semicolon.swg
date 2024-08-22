#global skipfmt

/**
In Swag, unlike in C/C++, there's no requirement to end statements with a semicolon (`;`). Typically, the end of a line (`end of line`) is sufficient to terminate a statement, resulting in cleaner and more concise syntax.
*/

#test
{
    // Declare two variables, x and y, of type s32 (signed 32-bit integer) and initialize them both to 1.
    var x, y: s32 = 1

    // Increment both x and y by 1.
    x += 1
    y += 1

    // Use the '@assert' intrinsic to verify the correctness of the values of x and y.
    @assert(x == 2)     // Ensure that x equals 2, raising an error if it does not.
    @assert(y == x)     // Ensure that y equals x (which should be 2).
}

/**
Although semicolons are optional, you can still use them when preferred or when they enhance readability in certain situations.
*/
#test
{
    // Note that type inference is used here, so the types of x and y are not explicitly specified.
    var x, y = 0;  // Both variables are initialized to 0, with their types inferred automatically.

    // The short syntax below applies the same operation to both variables.
    x, y += 1;     // Both x and y are incremented by 1.
}

/**
Semicolons can be particularly useful when writing multiple statements on the same line. This can make the code more compact, though it should be used sparingly to maintain readability.
*/
#test
{
    // Two statements on the same line, separated by a semicolon.
    var x = 0; var y = 0

    // Increment both x and y on the same line.
    x += 1; y += 1

    // Validate the values of x and y on the same line.
    @assert(x == 1); @assert(y == 1)
}
