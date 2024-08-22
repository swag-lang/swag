/**
If you use `const` instead of `var` or `let`, the value must be **known by the compiler** at compile time. This means that the value of a `const` is evaluated during compilation, and there's no runtime memory footprint for simple types like integers or strings. The compiler simply replaces the use of these constants with their values wherever they are used in the code.
*/

#test
{
    // These are constants, not variables. Once declared, they cannot be modified.
    const a = 666  // 'a' is a constant integer known at compile time.
    #assert a == 666

    const b: string = "string"  // 'b' is a constant string known at compile time.
    #assert b == "string"
}

/**
Constants in Swag can also have more complex types, such as arrays or structures. In such cases, there is a memory footprint because these constants are stored in the data segment of the program. This also means that you can take the address of these constants at runtime.
*/

/**
This is our first static array. It contains 3 elements, and the type of the elements is `s32` (signed 32-bit integer).
*/
#test
{
    const a: [3] s32 = [0, 1, 2]  // 'a' is a constant array of 3 integers.

    let ptr = &a[0]       // Take the address of the first element of the array.
    @assert(ptr[0] == 0)  // Access the first element through the pointer.
    @assert(ptr[2] == 2)  // Access the third element through the pointer.

    // Since 'a' is a constant, we can also perform compile-time checks.
    #assert a[0] == 0
    #assert a[1] == 1
    #assert a[2] == 2
}

/**
Here is an example of a multidimensional array declared as a constant. We will cover arrays in more detail later.
*/
#test
{
    const M4x4: [4, 4] f32 = [
    [1, 0, 0, 0],  // First row
    [0, 1, 0, 0],  // Second row
    [0, 0, 1, 0],  // Third row
    [0, 0, 0, 1]]  // Fourth row
}

/**
The key difference between `let` and `const` is that the value of a `const` must be known at compile time, whereas the value of a `let` can be determined dynamically at runtime. However, both `let` and `const` require that the variable or constant is assigned exactly once and cannot be reassigned.
*/
