/**
# Explicit Cast
Sometimes, it's necessary to change the type of a value explicitly. This can be done using `cast(type)` to convert a value from one type to another.
*/

#test
{
    // 'x' is a floating-point number (f32 by default)
    let x = 1.0
    #assert @typeof(x) == f32

    // 'y' is explicitly cast to a 32-bit integer (s32)
    let y = cast(s32) x
    #assert @typeof(y) == s32
    @assert(y == 1)  // The floating-point value 1.0 is cast to the integer value 1
}

/**
# acast (Automatic Cast)
`acast` stands for `automatic cast`. It lets the compiler automatically determine and perform the cast to match the type on the left-hand side of the assignment.
*/
#test
{
    let x: f32 = 1.0
    let y: s32 = acast x  // Automatically cast 'x' to 's32'
    #assert @typeof(y) == s32
    @assert(y == 1)
}

/**
# Bitcast
The `#bit` modifier allows you to reinterpret the bits of a value as a different type without changing the underlying bit pattern. This operation is called a **bitcast** and is only valid when the source and destination types have the same size.
*/
#test
{
    let x: f32 = 1.0
    let y: u32 = cast(u32) #bit x  // Reinterpret the bits of 'x' as a 'u32'
    @assert(y == 0x3f800000)  // 1.0 in IEEE 754 floating-point format equals 0x3f800000 in hex

    // Casting back to the original type should yield the original value
    #assert cast(u32) #bit 1.0 == 0x3f800000
    #assert cast(f32) #bit 0x3f800000 == 1.0  // Reinterpreting the bits back to 'f32' gives 1.0
}

/**
This example demonstrates the reverse operation, where we take an integer representing a bit pattern and reinterpret it as a floating-point number.
*/
#test
{
    let rawBits: u32 = 0x40490FDB              // Hexadecimal representation of the float 3.1415927
    let pi: f32      = cast(f32) #bit rawBits  // Interpret the bit pattern as a floating-point number
    @assert(pi == 3.1415927)                   // This should now represent the value of pi as a floating-point number

    // Verifying that casting back to the original bit pattern restores the initial value
    let backToBits: u32 = cast(u32) #bit pi
    @assert(backToBits == 0x40490FDB)
}

/**
# Implicit Casts
Swag can sometimes automatically cast (implicit cast) from one type to another without requiring an explicit cast. However, this only happens when there is no loss of precision.
*/
#test
{
    // Implicit cast from 8-bit signed integer (s8) to 16-bit signed integer (s16)
    let x: s16 = 1's8  // Safe conversion, no loss of precision

    // Implicit cast from 16-bit signed integer (s16) to 32-bit signed integer (s32)
    let y: s32 = 1's16  // Safe conversion, no loss of precision

    // However, the reverse would generate an error, as casting from a larger type to a smaller one
    // can lead to loss of precision and requires an explicit cast.
    // Uncommenting the following lines would cause a compilation error:

    // let z0: s16 = 1
    // let z1: s8 = z0  // Error: Implicit cast from 's16' to 's8' is not allowed

    // Instead, you would need to use an explicit cast like this:
    // let z1: s8 = cast(s8) z0
}
