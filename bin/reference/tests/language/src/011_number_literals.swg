/**
Integers in *decimal*, *hexadecimal* or *binary* forms.
*/
#test
{
    const a: u32 = 123456         // Decimal
    const b: u32 = 0xFFFF         // Hexadecimal, with '0x'
    const c: u32 = 0b00001111     // Binary, with '0b'
    @assert(a == 123456)
    @assert(b == 65535)
    @assert(c == 15)
}

/**
You can separate the digits with the `_` character.
*/
#test
{
    const a: u32 = 123_456
    const b: u32 = 0xF_F_F_F
    const c: u32 = 0b0000_1111
    @assert(a == 123456)
    @assert(b == 65_535)
    @assert(c == 15)
}

/**
The default type of an hexadecimal number or a binary number is `u32` or `u64` depending on its value.
*/
#test
{
    // The compiler will deduce that the type of 'a' is 'u32'.
    const a = 0xFF
    #assert @typeof(a) == u32

    // The compiler will deduce that the type of 'b' is 'u64' because the constant
    // is too big for 32 bits.
    const b = 0xF_FFFFF_FFFFFF
    #assert @typeof(b) == u64

    const c = 0b00000001
    #assert @typeof(c) == u32
    const d = 0b00000001_00000001_00000001_00000001_00000001
    #assert @typeof(d) == u64
}

/**
A boolean is `true` or `false`. Note again that as constants are known at compile time, we can  use `#assert` to check the values.
*/
#test
{
    const a = true
    #assert a == true

    const b, c = false
    #assert b == false
    #assert c == false
}

/**
A floating point value has the usual C/C++ form.
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)
    #assert @typeof(a) == f32

    let b = 0.11
    @assert(b == 0.11)

    let c = 15e2
    @assert(c == 15e2)

    let d = 15e+2
    @assert(d == 15e2)

    let e = -1E-1
    @assert(e == -0.1)
}

/**
By default, a floating point value is `f32`, not `f64` (aka `double`) like in C/C++.
*/
#test
{
    let a = 1.5
    @assert(a == 1.5)
    #assert @typeof(a) == f32
    #assert @typeof(a) != f64
}

/**
# Suffix
You can also force the type by adding a **suffix** to a literal number. We will see later that this syntax is also used for generic arguments.
*/
#test
{
    // Declare 'a' to be a 'f64' variable assigned to '1.5'
    let a = 1.5'f64
    @assert(a == 1.5)
    @assert(a == 1.5'f64)
    #assert @typeof(a) == f64

    // 'b' is a new variable of type 'u8' initialized with '10'.
    let b = 10'u8
    @assert(b == 10)
    #assert @typeof(b) == u8

    let c = 1'u32
    #assert @typeof(c) == u32
}
