#global skipfmt

/**
In Swag, unlike in C/C++, there is no need to end a statement with a semicolon (`;`). In most cases, the end of a line (`end of line`) is sufficient to terminate a statement. This makes the syntax cleaner and more concise.
*/

#test
{
    // Declare two variables x and y of type s32 (signed 32-bit integer) and initialize them both to 1.
    var x, y: s32 = 1

    // Increment x and y by 1.
    x += 1
    y += 1

    // The '@assert' intrinsic is used to verify that the values of x and y are correct.
    @assert(x == 2)     // Verify that x is equal to 2, and raise an error if not.
    @assert(y == x)     // Verify that y is equal to x (which should be 2).
}

/**
While semicolons are not mandatory, you can still use them if you prefer or if it improves readability in specific scenarios.
*/
#test
{
    // Notice that the type of x and y is not explicitly specified here due to type inference.
    var x, y = 0;  // The variables are initialized to 0, with the type inferred automatically.

    // The following line uses a short syntax to apply the same operation to both variables.
    x, y += 1;     // Both x and y are incremented by 1.
}

/**
Semicolons can be useful when you want to write multiple statements on the same line. This can make your code more compact, but should be used sparingly for the sake of readability.
*/
#test
{
    // Two instructions on the same line, separated by a semicolon.
    var x = 0; var y = 0

    // Increment both x and y on the same line.
    x += 1; y += 1

    // Validate the values of x and y on the same line.
    @assert(x == 1); @assert(y == 1)
}
