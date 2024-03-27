/**
In Swag, there's no need to end a statement with `;` like in C/C++. Most of the time a `end of line` is enough.
*/

#test
{
    // Declare two variables x and y of type s32 (signed 32 bits), and initialize them to 1.
    var x, y: s32 = 1

    // Increment x and y by 1.
    x += 1
    y += 1

    // When running the tests, the '@assert' intrinsic will verify that those values are correct.
    @assert(x == 2)     // Verify that x is equal to 2, and raise an error if not.
    @assert(y == x)     // Verify that y is equal to x.
}

/**
The semicolons are not mandatory, but it's possible to use them if you want.
*/
#test
{
    // You can notice that the type of x and y is not specified here.
    // This is due to type inference (we will see that later).
    var x, y = 0;

    // You can also notice the short syntax to do the same operation on multiple variables at once.
    x, y += 1;
}

/**
Semicolons can be usefull if you want to do multiple things on the same line.
*/
#test
{
    // Two instructions on the same line separated with ';'
    var x = 0; var y = 0

    // Two instructions on the same line separated with ';'
    x += 1; y += 1

    // Two instructions on the same line separated with ';'
    @assert(x == 1); @assert(y == 1)
}