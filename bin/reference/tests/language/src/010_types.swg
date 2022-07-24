#test
{
    // This are all signed integers types
    {
        var a: s8 = -1      // 8 bits signed integer
        var b: s16 = -2     // 16 bits signed integer
        var c: s32 = -3     // 32 bits signed integer
        var d: s64 = -4     // 64 bits signed integer
        var e: int = -4     // 64 bits signed integer

        // The instrinsic '@assert' will panic at runtime if the enclosed expression is false
        @assert(a == -1)
        @assert(b == -2)
        @assert(c == -3)
        @assert(d == -4)

        // The instrinsic '@sizeof' gives the size, in bytes, of a variable
        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 2)
        @assert(@sizeof(c) == 4)
        @assert(@sizeof(d) == 8)
        @assert(@sizeof(e) == 8)
    }

    // This are all unsigned integers types
    {
        var a: u8 = 1       // 8 bits unsigned integer
        var b: u16 = 2      // 16 bits unsigned integer
        var c: u32 = 3      // 32 bits unsigned integer
        var d: u64 = 4      // 64 bits unsigned integer
        var e: uint = 4     // 64 bits unsigned integer

        @assert(a == 1)
        @assert(b == 2)
        @assert(c == 3)
        @assert(d == 4)

        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 2)
        @assert(@sizeof(c) == 4)
        @assert(@sizeof(d) == 8)
        @assert(@sizeof(e) == 8)
    }

    // This are all float types
    {
        var a: f32 = 3.14       // 32 bits floating point value
        var b: f64 = 3.14159    // 64 bits floating point value

        @assert(a == 3.14)
        @assert(b == 3.14159)

        @assert(@sizeof(a) == 4)
        @assert(@sizeof(b) == 8)
    }

    // The boolean type
    {
        var a: bool = true      // Stored in 1 byte
        var b: bool = false     // Stored in 1 byte

        @assert(a == true)
        @assert(b == false)

        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 1)
    }

    // The string type.
    // Strings are UTF8, and are stored as two 64 bits (the pointer to the value and the string length in bytes)
    {
        var a: string = "string 是"
        @assert(a == "string 是")
        @assert(@sizeof(a) == 2 * @sizeof(*void))
    }

    // The 'rune' type uses the string syntax, postfix with the type 'rune'.
    // It's a 32 bits unicode code point.
    {
        var a: rune = "是"'rune
        @assert(a == "是"'rune)
        @assert(@sizeof(a) == 4)
    }

    // Swag has type reflection.
    // You can use '@typeof' to get the type of an expression.
    {
        var a = 0
        var b: @typeof(a) = 1   // 'b' will have the same type as 'a'
        @assert(@typeof(a) == @typeof(b))
        @assert(@typeof(a) == s32)

        // As the types of 'a' and 'b' are known at compile time, we can use #assert instead
        // The #assert will be done by the compiler, and will not generate runtime code (unlike '@assert')
        #assert @typeof(a) == @typeof(b)
        #assert @typeof(a) == s32
    }

    // Types are values, at compile time and at runtime.
    {
        x := @typeof(s32)
        @assert(x == s32)
        @assert(x.name == "s32")
    }
}