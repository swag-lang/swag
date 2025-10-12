/**
# Number Representations

Integers can be written in multiple formats: *decimal*, *hexadecimal*, or *binary*.
These representations allow you to express numbers in the format that best fits
your use case or domain requirements.
*/
#test
{
    const a: u32 = 123456         // Decimal format
    const b: u32 = 0xFFFF         // Hexadecimal, prefixed with '0x' (represents 65535)
    const c: u32 = 0b00001111     // Binary, prefixed with '0b' (represents 15)

    @assert(a == 123456)
    @assert(b == 65535)
    @assert(c == 15)
}

/**
# Digit Separators

Numeric literals can use the underscore ('_') as a digit separator for better
readability. Separators do not affect the numeric value.
*/
#test
{
    const a: u32 = 123_456         // Decimal with separators
    const b: u32 = 0xF_F_F_F       // Hexadecimal with separators
    const c: u32 = 0b0000_1111     // Binary with separators

    @assert(a == 123456)
    @assert(b == 65535)
    @assert(c == 15)
}

/**
# Default Integer Types

In Swag, hexadecimal or binary literals default to type 'u32' if the value fits
within 32 bits. If the literal exceeds 32 bits, it is automatically inferred as 'u64'.
*/
#test
{
    const a = 0xFF // Fits in 32 bits → inferred as 'u32'
    #assert(#typeof(a) == u32)

    const b = 0xF_FFFFF_FFFFFF // Exceeds 32 bits → inferred as 'u64'
    #assert(#typeof(b) == u64)

    const c = 0b00000001 // Within 32 bits → inferred as 'u32'
    #assert(#typeof(c) == u32)

    const d = 0b00000001_00000001_00000001_00000001_00000001 // Exceeds 32 bits → 'u64'
    #assert(#typeof(d) == u64)
}

/**
# Booleans

A 'bool' type can hold either 'true' or 'false'. Since constants are known at compile
time, you can use '#assert' to verify their values during compilation.
*/
#test
{
    const a = true
    #assert(a == true)

    const b, c = false
    #assert(b == false)
    #assert(c == false)
}

/**
# Floating-Point Values

Floating-point literals use standard C-style notation. This makes them familiar and
easy to read for developers coming from C or C++ backgrounds.
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)
    #assert(#typeof(a) == f32) // Default float type is 'f32'

    let b = 0.11
    @assert(b == 0.11)

    let c = 15e2
    @assert(c == 15e2) // Equivalent to 1500

    let d = 15e+2
    @assert(d == 15e2)

    let e = -1E-1
    @assert(e == -0.1)
}

/**
# Default Floating-Point Type

By default, floating-point literals are of type 'f32'. This differs from C and C++,
where floating-point literals default to 'double' ('f64').
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)
    #assert(#typeof(a) == f32)
    #assert(#typeof(a) != f64)
}

/**
# Literal Suffix

You can specify the type of a literal explicitly by adding a **suffix** to it. This
is useful when a specific type is required, such as 'f64' or 'u8'.
*/
#test
{
    let a = 1.5'f64 // Explicitly declare 'a' as 'f64'
    @assert(a == 1.5)
    @assert(a == 1.5'f64)
    #assert(#typeof(a) == f64)

    let b = 10'u8 // Declare 'b' as 'u8'
    @assert(b == 10)
    #assert(#typeof(b) == u8)

    let c = 1'u32 // Explicitly typed as 'u32'
    #assert(#typeof(c) == u32)
}
