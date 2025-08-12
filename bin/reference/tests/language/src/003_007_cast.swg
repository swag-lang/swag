/**
# Explicit Cast with 'cast'
Explicit casting is necessary when you need to convert a value from one type to another manually. This can be achieved using the `cast(type) value` syntax, which transforms the `value` into the specified `type`.
*/
#test
{
    // 'x' is initialized as a floating-point number (f32 by default)
    let x = 1.0
    #assert(#typeof(x) == f32)

    // 'y' is explicitly cast to a 32-bit signed integer (s32)
    let y = cast(s32) x
    #assert(#typeof(y) == s32)
    @assert(y == 1)                            // The floating-point value 1.0 is cast to the integer value 1
}

/**
# Automatic Cast with 'cast()'
A `cast()` without an expression stands for `automatic cast`, allowing the compiler to automatically determine and perform the cast to match the type on the left-hand side of the assignment.
*/
#test
{
    let x: f32 = 1.0
    let y: s32 = cast() x                       // Automatically cast 'x' to 's32'
    #assert(#typeof(y) == s32)
    @assert(y == 1)
}

/**
This also works for a function call argument.
*/
func testAutoCast(x: s32)
{
    @assert(x == 1)
}

#test
{
    testAutoCast(cast() 1.4)        // Automatic cast to the type 's32' of 'x'
}

/**
# Bitcast
The `bit` cast mode enables bit-level reinterpretation of a value's type without altering the underlying bit pattern. This operation, known as a **bitcast**, is only valid when the source and destination types share the same size.
*/
#test
{
    let x: f32 = 1.0
    let y: u32 = cast #bit (u32) x              // Reinterpret the bits of 'x' as a 'u32'
    @assert(y == 0x3f800000)                   // 1.0 in IEEE 754 floating-point format equals 0x3f800000 in hex

    // Casting back to the original type should yield the original value
    #assert(cast #bit (u32) 1.0 == 0x3f800000)
    #assert(cast #bit (f32) 0x3f800000 == 1.0)   // Reinterpreting the bits back to 'f32' gives 1.0
}

/**
This example demonstrates the reverse operation, where an integer representing a bit pattern is reinterpreted as a floating-point number.
*/
#test
{
    let rawBits: u32 = 0x40490FDB              // Hexadecimal representation of the float 3.1415927
    let pi: f32      = cast #bit (f32) rawBits  // Interpret the bit pattern as a floating-point number
    @assert(pi == 3.1415927)                   // This now represents the value of pi as a floating-point number

    // Verifying that casting back to the original bit pattern restores the initial value
    let backToBits: u32 = cast #bit (u32) pi
    @assert(backToBits == 0x40490FDB)
}

/**
# Implicit Casts
Swag allows automatic type conversions, known as *implicit casts*, when assigning a value of one type to a variable of another type. This occurs without requiring an explicit `cast` statement. However, implicit casts are only permitted when there is no risk of precision loss or range overflow, ensuring that the value remains intact and no data is lost during the conversion.

## Implicit Cast Rules
1. **Widening Conversions**: Implicit casts are allowed when converting a smaller type to a larger type, such as from `s8` to `s16`, or from `f32` to `f64`. These conversions are safe because the larger type can represent all possible values of the smaller type.

2. **Sign Preservation**: When implicitly casting between signed and unsigned types, Swag ensures that no data loss occurs by verifying that the value can be represented in both types. Implicit casts from unsigned to signed types (and vice versa) are only allowed when the value is positive and within the range of the target type.

3. **No Implicit Narrowing**: Swag does not permit implicit casts that could potentially lose data or precision, such as from `s32` to `s8`. These narrowing conversions require an explicit cast to indicate that the developer is aware of the potential data loss.

## Examples of Implicit Casts
Let's explore some examples to illustrate these rules.
*/
#test
{
    // Implicit cast from 8-bit signed integer (s8) to 16-bit signed integer (s16)
    let x: s16 = 1's8                         // Safe conversion, no loss of precision

    // Implicit cast from 16-bit signed integer (s16) to 32-bit signed integer (s32)
    let y: s32 = 1's16                        // Safe conversion, no loss of precision

    // Implicit cast from 32-bit signed integer (s32) to 64-bit signed integer (s64)
    let z: s64 = 1's32                        // Safe conversion, no loss of precision

    // Implicit cast from 8-bit unsigned integer (u8) to 16-bit unsigned integer (u16)
    let a: u16 = 255'u8                       // Safe conversion, no loss of precision

    // Implicit cast from 16-bit unsigned integer (u16) to 32-bit unsigned integer (u32)
    let b: u32 = 65535'u16                    // Safe conversion, no loss of precision

    // Implicit cast from 32-bit unsigned integer (u32) to 64-bit unsigned integer (u64)
    let c: u64 = 4294967295'u32               // Safe conversion, no loss of precision

    // Implicit cast from 32-bit floating-point (f32) to 64-bit floating-point (f64)
    let d: f64 = 1.23'f32                     // Safe conversion, f64 can represent all f32 values accurately
}

/**
## Examples Where Implicit Casts Are Not Allowed
There are cases where implicit casts are not permitted due to the risk of data loss or precision issues. In such situations, Swag requires an explicit cast to ensure that the developer is aware of and accepts the risks.

Additionally, the cast mode `unsafe` can be used in explicit casts to indicate that the value may lose some precision without raising an error.
*/
#test
{
    // Implicit cast from 16-bit signed integer (s16) to 8-bit signed integer (s8)
    // This would generate a compilation error because s8 cannot represent all s16 values.
    // Uncommenting the following lines would cause an error:

    // let z0: s16 = 100
    // let z1: s8 = z0                          // Error: Implicit cast from 's16' to 's8' is not allowed

    // To perform this cast, an explicit cast is required:
    let z0: s16 = 256
    let z1: s8 = cast #wrap (s8) z0          // Explicit cast needed to convert from s16 to s8
    @assert(z1 == 0)                            // This may cause data loss as 256 cannot be represented in s8 (max is 127)

    // Implicit cast from unsigned to signed type where the value is out of range
    let u_val: u16 = 65535
    // let s_val: s16 = u_val                   // Error: Implicit cast from 'u16' to 's16' is not allowed due to potential data loss

    // To perform this cast, an explicit cast is required, with the risk of data loss:
    let s_val: s16 = cast #wrap (s16) u_val  // This could result in an unexpected negative value
    @assert(s_val == -1)                        // 65535 in u16 becomes -1 in s16 due to overflow

    // Implicit cast from f64 to f32, which can lose precision
    let large_float: f64 = 1.23456789012345611111
    // let smaller_float: f32 = large_float     // Error: Implicit cast from 'f64' to 'f32' is not allowed due to precision loss

    // Explicit cast is required when converting from f64 to f32
    let smaller_float: f32 = cast(f32) large_float
}
