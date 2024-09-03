/**
# Number Representations
Integers can be written in multiple formats: *decimal*, *hexadecimal*, or *binary*. These
different representations allow you to express numbers in the format that best suits your needs.
*/
#test
{
    const a: u32 = 123456          // Decimal format representation of the integer
    const b: u32 = 0xFFFF          // Hexadecimal format, prefixed with '0x' (representing 65535)
    const c: u32 = 0b00001111      // Binary format, prefixed with '0b' (representing 15)

    @assert(a == 123456)           // Verifies that 'a' holds the correct decimal value
    @assert(b == 65535)            // Verifies that 'b' holds the correct hexadecimal value
    @assert(c == 15)               // Verifies that 'c' holds the correct binary value
}

/**
# Digit Separators
Numeric literals can be more readable by using the `_` character as a digit separator. This is 
particularly useful for large numbers, without affecting their value.
*/
#test
{
    const a: u32 = 123_456         // Decimal with digit separators for better readability
    const b: u32 = 0xF_F_F_F       // Hexadecimal with digit separators
    const c: u32 = 0b0000_1111     // Binary with digit separators

    @assert(a == 123456)           // Verifies that 'a' holds the correct value
    @assert(b == 65535)            // Verifies that 'b' holds the correct value
    @assert(c == 15)               // Verifies that 'c' holds the correct value
}

/**
# Default Integer Types
In Swag, hexadecimal or binary numbers default to `u32` if they fit within 32 bits. If the value 
exceeds 32 bits, it is automatically inferred as `u64`.
*/
#test
{
    const a = 0xFF                 // Compiler infers 'a' as 'u32' since value fits within 32 bits
    #assert #typeof(a) == u32      // Verifies that 'a' is of type 'u32'

    const b = 0xF_FFFFF_FFFFFF     // Large value exceeding 32 bits; inferred as 'u64'
    #assert #typeof(b) == u64      // Verifies that 'b' is of type 'u64'

    const c = 0b00000001           // Binary value within 32 bits, inferred as 'u32'
    #assert #typeof(c) == u32      // Verifies that 'c' is of type 'u32'

    const d = 0b00000001_00000001_00000001_00000001_00000001  // Exceeds 32 bits, inferred as 'u64'
    #assert #typeof(d) == u64      // Verifies that 'd' is of type 'u64'
}

/**
# Booleans
A boolean type can be either `true` or `false`. As constants are known at compile time, you 
can use `#assert` to verify their values during compilation.
*/
#test
{
    const a = true
    #assert a == true              // Compile-time check that 'a' holds the value true

    const b, c = false
    #assert b == false             // Compile-time check that 'b' holds the value false
    #assert c == false             // Compile-time check that 'c' holds the value false
}

/**
# Floating Point Values
Floating point values use the standard C/C++ notation for floating-point literals. This 
provides familiarity and ease of use for developers.
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)              // Verifies that 'a' holds the value 1.5
    #assert #typeof(a) == f32      // The type of 'a' is inferred to be 'f32'

    let b = 0.11
    @assert(b == 0.11)             // Verifies that 'b' holds the value 0.11

    let c = 15e2
    @assert(c == 15e2)             // Verifies that 'c' holds the value 15e2 (1500)

    let d = 15e+2
    @assert(d == 15e2)             // Verifies that 'd' holds the same value as 'c'

    let e = -1E-1
    @assert(e == -0.1)             // Verifies that 'e' holds the value -0.1
}

/**
# Default Floating Point Type
By default, floating-point literals are of type `f32`. This is different from languages like 
C/C++, where floating-point literals default to `double` (`f64`).
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)              // Verifies that 'a' holds the value 1.5
    #assert #typeof(a) == f32      // Verifies that 'a' is of type 'f32'
    #assert #typeof(a) != f64      // Verifies that 'a' is not of type 'f64'
}

/**
# Literal Suffix
To specify the type of a literal explicitly, you can add a **suffix** to the literal. This is useful 
when a specific type is required, such as `f64` or `u8`.
*/
#test
{
    let a = 1.5'f64                // Explicitly declare 'a' as 'f64' with the value 1.5
    @assert(a == 1.5)              // Verifies that 'a' holds the value 1.5
    @assert(a == 1.5'f64)          // Verifies that 'a' explicitly holds the value 1.5 as 'f64'
    #assert #typeof(a) == f64      // Verifies that 'a' is of type 'f64'

    let b = 10'u8                  // Declare 'b' as 'u8' and assign it the value 10
    @assert(b == 10)               // Verifies that 'b' holds the value 10
    #assert #typeof(b) == u8       // Verifies that 'b' is of type 'u8'

    let c = 1'u32
    #assert #typeof(c) == u32      // Verifies that 'c' is of type 'u32'
}
