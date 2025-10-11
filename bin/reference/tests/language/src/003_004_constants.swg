/**
# Constants with `const`
Using `const` implies that the value must be **known by the compiler** at compile time.
This ensures that the value is embedded directly into the compiled code, eliminating any runtime
memory usage for simple types like integers or strings. Essentially, the compiler replaces occurrences
of these constants with their respective values wherever they are referenced in the code, leading to
more optimized and efficient code execution.
*/

#test
{
    // These are immutable constants. Once declared, they are fixed and cannot be altered.
    const a = 666 // 'a' is an integer constant known and resolved at compile time.
    #assert(a == 666)
    // Assertion to verify the value of 'a'.

    const b: string = "string" // 'b' is a string constant known and resolved at compile time.
    #assert(b == "string")
    // Assertion to verify the value of 'b'.
}

/**
# Constants with Complex Types
Swag also supports constants with more complex data types, such as arrays or structures. When
declaring constants with these types, the data is stored in the program's data segment, which incurs
a memory footprint. Additionally, you can obtain the memory address of these constants at runtime,
allowing you to manipulate their values indirectly through pointers.
*/

/**
## Static Arrays
The following example demonstrates a static array, which is an array with a fixed size. This array
contains three elements of type `s32`, a signed 32-bit integer.
*/
#test
{
    const a: [3] s32 = [0, 1, 2] // 'a' is a constant static array with 3 signed 32-bit integers.

    let ptr = &a[0] // Obtain the memory address of the first element of the array.
    @assert(ptr[0] == 0) // Verify the first element via pointer dereferencing.
    @assert(ptr[2] == 2) // Verify the third element via pointer dereferencing.

    // Compile-time assertions to ensure the array's contents are as expected.
    #assert(a[0] == 0)
    // Verify the first element of the array at compile time.
    #assert(a[1] == 1)
    // Verify the second element of the array at compile time.
    #assert(a[2] == 2)
    // Verify the third element of the array at compile time.
}

/**
## Multidimensional Arrays
This example illustrates a multidimensional array declared as a constant. The array is a 4x4 matrix of
32-bit floating-point numbers (`f32`). We will explore arrays in further detail later in this documentation.
*/
#test
{
    const M4x4: [4, 4] f32 = [
        [1, 0, 0, 0], // First row of the matrix.
        [0, 1, 0, 0], // Second row of the matrix.
        [0, 0, 1, 0], // Third row of the matrix.
        [0, 0, 0, 1]] // 'M4x4' is a constant 4x4 matrix of 32-bit floating-point numbers.
}

/**
# Key Difference Between `let` and `const`
The primary distinction between `let` and `const` lies in the timing of value determination. The value
assigned to a `const` must be determined at compile time, meaning it is fixed and known before the program
runs. Conversely, a `let` allows for dynamic assignment, where the value can be computed and assigned
during runtime. Despite this, both `let` and `const` ensure that their values are assigned only once,
maintaining immutability.
*/
