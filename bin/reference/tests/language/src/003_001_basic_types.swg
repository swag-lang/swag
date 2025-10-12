/**
# Signed Integers

Swag provides various signed integer types: 's8', 's16', 's32', and 's64'.
These types represent signed integers with different bit widths, allowing both
positive and negative values within their respective ranges. Each type is designed
for efficient integer operations at varying levels of precision.
*/
#test
{
    let a: s8 = -1      // 8-bit signed integer, range: -128 to 127
    let b: s16 = -2     // 16-bit signed integer, range: -32,768 to 32,767
    let c: s32 = -3     // 32-bit signed integer, range: -2^31 to 2^31-1
    let d: s64 = -4     // 64-bit signed integer, range: -2^63 to 2^63-1

    @assert(a == -1)
    @assert(b == -2)
    @assert(c == -3)
    @assert(d == -4)

    @assert(#sizeof(a) == 1) // 'a' is an 's8' → 1 byte
    @assert(#sizeof(b) == 2) // 'b' is an 's16' → 2 bytes
    @assert(#sizeof(c) == 4) // 'c' is an 's32' → 4 bytes
    @assert(#sizeof(d) == 8) // 'd' is an 's64' → 8 bytes
}

/**
# Unsigned Integers

Swag also supports unsigned integer types: 'u8', 'u16', 'u32', and 'u64'.
These types can only represent non-negative values, making them ideal for
counting, indexing, and other operations where negative numbers are not applicable.
*/
#test
{
    let a: u8 = 1      // 8-bit unsigned integer, range: 0 to 255
    let b: u16 = 2     // 16-bit unsigned integer, range: 0 to 65,535
    let c: u32 = 3     // 32-bit unsigned integer, range: 0 to 2^32-1
    let d: u64 = 4     // 64-bit unsigned integer, range: 0 to 2^64-1

    @assert(a == 1)
    @assert(b == 2)
    @assert(c == 3)
    @assert(d == 4)

    @assert(#sizeof(a) == 1) // 'a' is a 'u8' → 1 byte
    @assert(#sizeof(b) == 2) // 'b' is a 'u16' → 2 bytes
    @assert(#sizeof(c) == 4) // 'c' is a 'u32' → 4 bytes
    @assert(#sizeof(d) == 8) // 'd' is a 'u64' → 8 bytes
}

/**
# Floating-Point Types

Swag supports floating-point types 'f32' and 'f64'. These represent single-
and double-precision floating-point numbers, suitable for calculations requiring
fractional values or higher precision.
*/
#test
{
    let a: f32 = 3.14        // 32-bit float (single precision)
    let b: f64 = 3.14159     // 64-bit float (double precision)

    @assert(a == 3.14)
    @assert(b == 3.14159)

    @assert(#sizeof(a) == 4) // 'a' is an 'f32' → 4 bytes
    @assert(#sizeof(b) == 8) // 'b' is an 'f64' → 8 bytes
}

/**
# Boolean Type

The boolean type 'bool' represents logical true or false values. In Swag,
a boolean occupies 1 byte of memory.
*/
#test
{
    let a: bool = true
    let b: bool = false

    @assert(a == true)
    @assert(b == false)

    @assert(#sizeof(a) == 1) // 'bool' → 1 byte
    @assert(#sizeof(b) == 1)
}

/**
# String Type

The 'string' type represents text. Strings in Swag are UTF-8 encoded and stored
as two 64-bit values: one for the data pointer and one for the length in bytes.
This structure ensures efficient text manipulation and full Unicode compatibility.
*/
#test
{
    let a: string = "string 是" // UTF-8 encoded string

    @assert(a == "string 是")
    @assert(#sizeof(a) == 2 * #sizeof(*void)) // Pointer + length
}

/**
# Rune Type

The 'rune' type represents a 32-bit Unicode code point. It stores a single
Unicode character and is ideal for per-character text operations across languages.
*/
#test
{
    let a: rune = '是' // Single Unicode code point

    @assert(a == '是')
    @assert(#sizeof(a) == 4) // 'rune' → 4 bytes (32 bits)
}

/**
# Type Reflection

Swag supports **type reflection** at both **compile time** and **runtime**.
This allows inspection and manipulation of types dynamically, enabling powerful
and introspective programming techniques.

Further details about type reflection are explored in later sections.
*/

/**
# Type Creation with '#decltype'

You can use '#decltype' to create a type based on an existing expression.
This is useful for inferring or mirroring the type of another variable dynamically,
improving reusability and reducing redundancy.
*/
#test
{
    let a  = 0                  // Type of 'a' inferred as 's32'
    let b: #decltype(a) = 1     // 'b' declared with same type as 'a'

    @assert(#typeof(a) == #typeof(b))
    @assert(#typeof(a) == s32)

    #assert(#typeof(a) == #typeof(b))
    #assert(#typeof(a) == s32)
}
