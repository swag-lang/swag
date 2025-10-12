/**
# Explicit Cast with 'cast'

Explicit casting converts a value from one type to another using the syntax
'cast(type) value'. This transformation changes the type of 'value' to the
specified 'type'.
*/
#test
{
    let x = 1.0 // Defaults to 'f32'
    #assert(#typeof(x) == f32)

    let y = cast(s32) x // Explicit cast to 's32'
    #assert(#typeof(y) == s32)
    @assert(y == 1)
}

/**
# Automatic Cast with 'cast()'

A 'cast()' without a type performs an **automatic cast**, allowing the compiler
to infer the target type based on the left-hand side of the assignment.
*/
#test
{
    let x: f32 = 1.0
    let y: s32 = cast() x
    #assert(#typeof(y) == s32)
    @assert(y == 1)
}

/**
Automatic casts can also be applied when passing function arguments.
*/
func testAutoCast(x: s32)
{
    @assert(x == 1)
}

#test
{
    testAutoCast(cast() 1.4) // Automatically cast to 's32'
}

/**
# Bit Cast

The '#bit' cast mode performs a **bit-level reinterpretation** of a value’s
type without altering its underlying bit pattern. Source and destination types
must have the same size.
*/
#test
{
    let x: f32 = 1.0
    let y: u32 = cast #bit (u32) x
    @assert(y == 0x3f800000) // 1.0 in IEEE 754 format

    #assert(cast #bit (u32) 1.0 == 0x3f800000)
    #assert(cast #bit (f32) 0x3f800000 == 1.0)
}

/**
Bitcasting also allows reinterpreting an integer’s bit pattern as a floating-point
value, or vice versa.
*/
#test
{
    let rawBits: u32 = 0x40490FDB
    let pi:      f32 = cast #bit (f32) rawBits
    @assert(pi == 3.1415927)

    let backToBits: u32 = cast #bit (u32) pi
    @assert(backToBits == 0x40490FDB)
}

/**
# Implicit Casts

Swag supports **implicit casts**, automatically converting between compatible
types when no data loss or precision loss can occur. These conversions happen
without requiring an explicit 'cast' statement.
*/

/**
## Implicit Cast Rules

1. **Widening conversions** — Allowed when converting from smaller to larger
   types, such as 's8' → 's16' or 'f32' → 'f64'.
2. **Sign preservation** — Implicit signed/unsigned conversions occur only if
   the value fits in the destination type.
3. **No narrowing conversions** — Conversions that could lose precision or range
   require an explicit cast.
*/
#test
{
    let x: s16 = 1's8
    let y: s32 = 1's16
    let z: s64 = 1's32

    let a: u16 = 255'u8
    let b: u32 = 65535'u16
    let c: u64 = 4294967295'u32

    let d: f64 = 1.23'f32
}

/**
# Disallowed Implicit Casts

Swag forbids implicit casts that may lose data or precision. In such cases, an
explicit cast is required. The '#wrap' or '#unsafe' cast modes can be used to
acknowledge potential data loss.
*/
#test
{
    // Explicit cast from 's16' to 's8' required
    let z0: s16 = 256
    let z1: s8 = cast #wrap (s8) z0
    @assert(z1 == 0)

    // Explicit cast from 'u16' to 's16' required
    let u_val: u16 = 65535
    let s_val: s16 = cast #wrap (s16) u_val
    @assert(s_val == -1)

    // Explicit cast from 'f64' to 'f32' required
    let large_float:   f64 = 1.23456789012345611111
    let smaller_float: f32 = cast(f32) large_float
}
