#test
{
    // These are all signed integers types `s8`, `s16`, `s32` and `s64`.
    {
        var a: s8  = -1     // 8 bits signed integer
        var b: s16 = -2     // 16 bits signed integer
        var c: s32 = -3     // 32 bits signed integer
        var d: s64 = -4     // 64 bits signed integer

        // Remember that the instrinsic '@assert' will raise an error at runtime if
        // the enclosed expression is false.
        @assert(a == -1)
        @assert(b == -2)
        @assert(c == -3)
        @assert(d == -4)

        // The instrinsic '@sizeof' gives the size, in bytes, of a variable.
        @assert(@sizeof(a) == 1)    // 'a' has type 's8', so this should be 1 byte.
        @assert(@sizeof(b) == 2)    // 'b' has type 's16', so this should be 2 bytes.
        @assert(@sizeof(c) == 4)
        @assert(@sizeof(d) == 8)
    }

    // These are all unsigned integers types `u8`, `u16`, `u32` and `u64`.
    {
        var a: u8  = 1      // 8 bits unsigned integer
        var b: u16 = 2      // 16 bits unsigned integer
        var c: u32 = 3      // 32 bits unsigned integer
        var d: u64 = 4      // 64 bits unsigned integer

        @assert(a == 1)
        @assert(b == 2)
        @assert(c == 3)
        @assert(d == 4)

        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 2)
        @assert(@sizeof(c) == 4)
        @assert(@sizeof(d) == 8)
    }

    // These are all floating point types `f32` and `f64`.
    {
        var a: f32 = 3.14       // 32 bits floating point value
        var b: f64 = 3.14159    // 64 bits floating point value

        @assert(a == 3.14)
        @assert(b == 3.14159)

        @assert(@sizeof(a) == 4)
        @assert(@sizeof(b) == 8)
    }

    // The boolean type `bool`.
    {
        var a: bool = true      // Stored in 1 byte
        var b: bool = false     // Stored in 1 byte

        @assert(a == true)
        @assert(b == false)

        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 1)
    }

    // The `string` type. Strings are **UTF8**, and are stored as two 64 bits (the pointer to the value and the string length in bytes). Note that a string literal also ends with a null byte like in C.
    {
        var a: string = "string 是"
        @assert(a == "string 是")
        @assert(@sizeof(a) == 2 * @sizeof(*void))
    }

    // The `rune` type uses the string syntax, postfix with the type `rune`. It's a 32 bits unicode code point.
    {
        var a: rune = "是"'rune
        @assert(a == "是"'rune)
        @assert(@sizeof(a) == 4)
    }
}

/**
# Type reflection
Swag has **type reflection** at **compile time** and at **runtime**. We will see that later in more details.
*/

#test
{
    // You can use `@decltype` to create a type based on an expression.
    {
        var a = 0                   // 'a' is inferred to have the 's32' type
        var b: @decltype(a) = 1     // 'b' will have the same type as 'a'

        // We can verify with '@typeof' that 'a' and 'b' have the same type.
        @assert(@typeof(a) == @typeof(b))
        @assert(@typeof(a) == s32)

        // As the types of 'a' and 'b' are known by the compiler, we can use '#assert' instead of '@assert'.
        // The '#assert' will be done at compile time, and will not generate code (unlike '@assert').
        #assert @typeof(a) == @typeof(b)
        #assert @typeof(a) == s32
    }

    // Short notice that types are also values, at compile time and at runtime.
    {
        x := @typeof(s32)   // 'x' is now a variable that contains a type
        @assert(x == s32)   // So it can be compared to a type

        // A type is a predefined struct, which contains some fields to inspect it.
        // You can for example get the name.
        @assert(x.name == "s32")

        // @typeof is not really necessary when there's no ambiguity about the expression.
        y := bool
        @assert(y == bool)
    }
}