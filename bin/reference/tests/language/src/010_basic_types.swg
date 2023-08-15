#test
{
    // These are all signed integers types `s8`, `s16`, `s32` and `s64`.
    {
        let a: s8  = -1     // 8 bits signed integer
        let b: s16 = -2     // 16 bits signed integer
        let c: s32 = -3     // 32 bits signed integer
        let d: s64 = -4     // 64 bits signed integer

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
        let a: u8  = 1      // 8 bits unsigned integer
        let b: u16 = 2      // 16 bits unsigned integer
        let c: u32 = 3      // 32 bits unsigned integer
        let d: u64 = 4      // 64 bits unsigned integer

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
        let a: f32 = 3.14       // 32 bits floating point value
        let b: f64 = 3.14159    // 64 bits floating point value

        @assert(a == 3.14)
        @assert(b == 3.14159)

        @assert(@sizeof(a) == 4)
        @assert(@sizeof(b) == 8)
    }

    // The boolean type `bool`.
    {
        let a: bool = true      // Stored in 1 byte
        let b: bool = false     // Stored in 1 byte

        @assert(a == true)
        @assert(b == false)

        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 1)
    }

    // The `string` type. Strings are **UTF8**, and are stored as two 64 bits (the pointer to the value and the string length in bytes). Note that a string literal also ends with a null byte like in C.
    {
        let a: string = "string 是"
        @assert(a == "string 是")
        @assert(@sizeof(a) == 2 * @sizeof(*void))
    }

    // The `rune` type is a 32 bits unicode code point.
    {
        let a: rune = `是`
        @assert(a == `是`)
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
        let a = 0                   // 'a' is inferred to have the 's32' type
        let b: @decltype(a) = 1     // 'b' will have the same type as 'a'

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
        let x = @typeof(s32)   // 'x' is now a variable that contains a type
        @assert(x == s32)   // So it can be compared to a type

        // A type is a predefined struct, which contains some fields to inspect it.
        // You can for example get the name.
        @assert(x.name == "s32")

        // @typeof is not really necessary when there's no ambiguity about the expression.
        let y = bool
        @assert(y == bool)
    }
}