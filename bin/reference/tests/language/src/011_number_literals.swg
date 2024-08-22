/**
In Swag, integers can be written in *decimal*, *hexadecimal*, or *binary* forms. These different representations allow you to express numbers in the format that best suits your needs.
*/
#test
{
    const a: u32 = 123456         // Decimal format
    const b: u32 = 0xFFFF         // Hexadecimal format, prefixed with '0x'
    const c: u32 = 0b00001111     // Binary format, prefixed with '0b'

    @assert(a == 123456)          // Verifies that 'a' holds the correct decimal value
    @assert(b == 65535)           // Verifies that 'b' holds the correct hexadecimal value
    @assert(c == 15)              // Verifies that 'c' holds the correct binary value
}

/**
You can separate digits within numeric literals using the `_` character for better readability, especially with large numbers. This does not affect the value of the number.
*/
#test
{
    const a: u32 = 123_456         // Decimal with digit separators
    const b: u32 = 0xF_F_F_F       // Hexadecimal with digit separators
    const c: u32 = 0b0000_1111     // Binary with digit separators

    @assert(a == 123456)           // Verifies that 'a' holds the correct value
    @assert(b == 65535)            // Verifies that 'b' holds the correct value
    @assert(c == 15)               // Verifies that 'c' holds the correct value
}

/**
The default type of a hexadecimal or binary number in Swag is `u32` if it fits within 32 bits. If the value exceeds 32 bits, the type is automatically inferred as `u64`.
*/
#test
{
    // The compiler will deduce that the type of 'a' is 'u32' since the value fits within 32 bits.
    const a = 0xFF
    #assert @typeof(a) == u32      // Verifies that 'a' is of type 'u32'

    // The compiler will deduce that the type of 'b' is 'u64' because the constant
    // is too large to fit within 32 bits.
    const b = 0xF_FFFFF_FFFFFF
    #assert @typeof(b) == u64      // Verifies that 'b' is of type 'u64'

    const c = 0b00000001           // Binary value within 32 bits
    #assert @typeof(c) == u32      // Verifies that 'c' is of type 'u32'

    const d = 0b00000001_00000001_00000001_00000001_00000001  // Binary value that exceeds 32 bits
    #assert @typeof(d) == u64      // Verifies that 'd' is of type 'u64'
}

/**
A boolean in Swag can be either `true` or `false`. Since constants are known at compile time, we can use `#assert` to check their values directly during compilation.
*/
#test
{
    const a = true
    #assert a == true              // Compile-time check that 'a' is true

    const b, c = false
    #assert b == false             // Compile-time check that 'b' is false
    #assert c == false             // Compile-time check that 'c' is false
}

/**
Floating point values in Swag follow the standard C/C++ notation for floating-point literals.
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)              // Verifies that 'a' holds the value 1.5
    #assert @typeof(a) == f32      // The type of 'a' is inferred to be 'f32'

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
By default, floating point literals in Swag are of type `f32`, unlike in C/C++, where they default to `double` (`f64`). 
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)              // Verifies that 'a' holds the value 1.5
    #assert @typeof(a) == f32      // Verifies that 'a' is of type 'f32'
    #assert @typeof(a) != f64      // Verifies that 'a' is not of type 'f64'
}

/**
# Suffix
You can explicitly set the type of a literal number by adding a **suffix**. This is useful when you need a specific type, such as `f64` or `u8`. We will explore how this syntax is used for generic arguments later on.
*/
#test
{
    // Declare 'a' to be of type 'f64' and assign it the value '1.5'
    let a = 1.5'f64
    @assert(a == 1.5)              // Verifies that 'a' holds the value 1.5
    @assert(a == 1.5'f64)          // Verifies that 'a' holds the value 1.5 explicitly as 'f64'
    #assert @typeof(a) == f64      // Verifies that 'a' is of type 'f64'

    // 'b' is a variable of type 'u8', initialized with the value '10'
    let b = 10'u8
    @assert(b == 10)               // Verifies that 'b' holds the value 10
    #assert @typeof(b) == u8       // Verifies that 'b' is of type 'u8'

    let c = 1'u32
    #assert @typeof(c) == u32      // Verifies that 'c' is of type 'u32'
}
