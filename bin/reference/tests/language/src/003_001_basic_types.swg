/**
# Signed Integers
Swag provides various signed integer types: `s8`, `s16`, `s32`, and `s64`. These types represent signed integers with different bit widths, allowing for both positive and negative values within their respective ranges. Each type is specifically designed to efficiently handle integer operations at varying levels of precision.
*/
#test
{
    let a: s8 = -1               // 8-bit signed integer, range: -128 to 127
    let b: s16 = -2              // 16-bit signed integer, range: -32,768 to 32,767
    let c: s32 = -3              // 32-bit signed integer, range: -2^31 to 2^31-1
    let d: s64 = -4              // 64-bit signed integer, range: -2^63 to 2^63-1

    @assert(a == -1)             // Verifies that 'a' holds the value -1.
    @assert(b == -2)             // Verifies that 'b' holds the value -2.
    @assert(c == -3)             // Verifies that 'c' holds the value -3.
    @assert(d == -4)             // Verifies that 'd' holds the value -4.

    @assert(#sizeof(a) == 1)     // 'a' is an s8, so its size is 1 byte.
    @assert(#sizeof(b) == 2)     // 'b' is an s16, so its size is 2 bytes.
    @assert(#sizeof(c) == 4)     // 'c' is an s32, so its size is 4 bytes.
    @assert(#sizeof(d) == 8)     // 'd' is an s64, so its size is 8 bytes.
}

/**
# Unsigned Integers
Swag also supports various unsigned integer types: `u8`, `u16`, `u32`, and `u64`. These types represent unsigned integers, which can only hold non-negative values, making them ideal for scenarios where negative numbers are not applicable.
*/
#test
{
    let a: u8 = 1               // 8-bit unsigned integer, range: 0 to 255
    let b: u16 = 2              // 16-bit unsigned integer, range: 0 to 65,535
    let c: u32 = 3              // 32-bit unsigned integer, range: 0 to 2^32-1
    let d: u64 = 4              // 64-bit unsigned integer, range: 0 to 2^64-1

    @assert(a == 1)             // Verifies that 'a' holds the value 1.
    @assert(b == 2)             // Verifies that 'b' holds the value 2.
    @assert(c == 3)             // Verifies that 'c' holds the value 3.
    @assert(d == 4)             // Verifies that 'd' holds the value 4.

    @assert(#sizeof(a) == 1)    // 'a' is a u8, so its size is 1 byte.
    @assert(#sizeof(b) == 2)    // 'b' is a u16, so its size is 2 bytes.
    @assert(#sizeof(c) == 4)    // 'c' is a u32, so its size is 4 bytes.
    @assert(#sizeof(d) == 8)    // 'd' is a u64, so its size is 8 bytes.
}

/**
# Floating-Point Types
Swag supports floating-point types `f32` and `f64`. These types represent single-precision and double-precision floating-point numbers, respectively. They are crucial for calculations requiring fractional values and greater precision.
*/
#test
{
    let a: f32 = 3.14           // 32-bit floating-point value (single-precision)
    let b: f64 = 3.14159        // 64-bit floating-point value (double-precision)

    @assert(a == 3.14)          // Verifies that 'a' holds the value 3.14.
    @assert(b == 3.14159)       // Verifies that 'b' holds the value 3.14159.

    @assert(#sizeof(a) == 4)    // 'a' is an f32, so its size is 4 bytes.
    @assert(#sizeof(b) == 8)    // 'b' is an f64, so its size is 8 bytes.
}

/**
# Boolean Type
The boolean type `bool` is used to represent true or false values. In Swag, a boolean is stored as a 1-byte value, offering a straightforward way to handle binary logic within your programs.
*/
#test
{
    let a: bool = true           // Boolean value, representing 'true'
    let b: bool = false          // Boolean value, representing 'false'

    @assert(a == true)           // Verifies that 'a' is true.
    @assert(b == false)          // Verifies that 'b' is false.

    @assert(#sizeof(a) == 1)     // The size of a boolean is 1 byte.
    @assert(#sizeof(b) == 1)     // The size of a boolean is 1 byte.
}

/**
# String Type
The `string` type represents text. In Swag, strings are **UTF-8** encoded and are stored as two 64-bit values: one for the pointer to the data and one for the length in bytes. Strings in Swag are designed to handle text efficiently, including international characters, while ensuring compatibility with common text processing routines.
*/
#test
{
    let a: string = "string 是"  // A string containing UTF-8 encoded characters

    @assert(a == "string 是")    // Verifies that 'a' holds the correct string.
    @assert(#sizeof(a) == 2 * #sizeof(*void)) // A string is stored as two 64-bit values (pointer and length).
}

/**
# Rune Type
The `rune` type in Swag represents a 32-bit Unicode code point. It is used to store individual Unicode characters, allowing for the manipulation of text at the character level, supporting a wide range of languages and symbols.
*/
#test
{
    let a: rune = `是`             // A single Unicode character, represented by a 32-bit code point

    @assert(a == `是`)              // Verifies that 'a' holds the correct Unicode code point.
    @assert(#sizeof(a) == 4)        // The size of a rune is 4 bytes (32 bits).
}

/**
# Type Reflection
Swag supports **type reflection** both at **compile time** and at **runtime**. This powerful feature allows for the inspection and manipulation of types dynamically, enabling more flexible and introspective programming paradigms. Further details on how to leverage type reflection will be explored in subsequent sections.
*/

/**
# Type Creation with #decltype
You can use `#decltype` to create a type based on an expression. This is useful for cases where you want to infer or mirror the type of a variable dynamically, promoting code reusability and reducing redundancy.
*/
#test
{
    let a  = 0                  // The type of 'a' is inferred to be 's32'.
    let b: #decltype(a) = 1     // 'b' is declared with the same type as 'a' (which is 's32').

    @assert(#typeof(a) == #typeof(b)) // Verifies that 'a' and 'b' have the same type.
    @assert(#typeof(a) == s32)        // Verifies that the type of 'a' is 's32'.

    #assert #typeof(a) == #typeof(b)  // Compile-time validation using '#assert'.
    #assert #typeof(a) == s32
}

/**
# Types as Values
Types in Swag are also treated as values, both at compile time and at runtime. This allows types to be manipulated, compared, and inspected just like any other value. This feature is particularly useful in metaprogramming and dynamic type manipulation scenarios.
*/
#test
{
    let x = #typeof(s32)        // 'x' is now a variable that holds a type (in this case, 's32').
    @assert(x == s32)           // Verifies that 'x' is equal to the type 's32'.

    @assert(x.name == "s32")    // Retrieves and verifies the name of the type 's32'.

    let y = bool                // 'y' is assigned the boolean type
    @assert(y == bool)          // Verifies that 'y' holds the type 'bool'.
}
