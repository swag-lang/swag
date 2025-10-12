/**
# Constants with 'const'

Using 'const' means the value must be **known by the compiler** at compile time.
The compiler embeds the constant’s value directly into the compiled code, removing
any runtime memory usage for simple types like integers or strings.

In other words, the compiler replaces every occurrence of a constant with its
literal value, leading to optimized and efficient code execution.
*/
#test
{
    // Immutable compile-time constants
    const a = 666
    #assert(a == 666)

    const b: string = "string"
    #assert(b == "string")
}

/**
# Constants with Complex Types

Swag also supports constants with complex data types, such as arrays and
structures. When declared, the data is stored in the program's data segment,
which occupies memory. The memory address of such constants can be accessed
at runtime, allowing indirect manipulation through pointers.
*/

/**
## Static Arrays

A static array has a fixed size. In this example, the constant array 'a' contains
three elements of type 's32' (signed 32-bit integers).
*/
#test
{
    const a: [3] s32 = [0, 1, 2]

    let ptr = &a[0]
    @assert(ptr[0] == 0)
    @assert(ptr[2] == 2)

    // Compile-time verification of array contents
    #assert(a[0] == 0)
    #assert(a[1] == 1)
    #assert(a[2] == 2)
}

/**
## Multidimensional Arrays

This example demonstrates a constant 4×4 matrix of 32-bit floating-point values
('f32'). Arrays will be explored in more detail later in this documentation.
*/
#test
{
    const M4x4: [4, 4] f32 = [
        [1, 0, 0, 0],
        [0, 1, 0, 0],
        [0, 0, 1, 0],
        [0, 0, 0, 1]]
}

/**
# Key Difference Between 'let' and 'const'

The key distinction between 'let' and 'const' lies in **when** their values are
determined:

- A 'const' value is fixed and known **at compile time**.
- A 'let' value can be assigned **at runtime**, allowing computation before assignment.

Despite this difference, both 'let' and 'const' enforce immutability — once a value
is assigned, it cannot be changed.
*/
