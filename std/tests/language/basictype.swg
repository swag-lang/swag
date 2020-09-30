#test
{
    // This are all signed integers types
    {
        var a: s8 = -1
        var b: s16 = -2
        var c: s32 = -3
        var d: s64 = -4

        @assert(a == -1)
        @assert(b == -2)
        @assert(c == -3)
        @assert(d == -4)
        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 2)
        @assert(@sizeof(c) == 4)
        @assert(@sizeof(d) == 8)
    }

    // This are all unsigned integers types
    {
        var a: u8 = 1
        var b: u16 = 2
        var c: u32 = 3
        var d: u64 = 4

        @assert(a == 1)
        @assert(b == 2)
        @assert(c == 3)
        @assert(d == 4)
        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 2)
        @assert(@sizeof(c) == 4)
        @assert(@sizeof(d) == 8)
    }

    // This are all float types
    {
        var a: f32 = 3.14
        var b: f64 = 3.14159

        @assert(a == 3.14)
        @assert(b == 3.14159)
        @assert(@sizeof(a) == 4)
        @assert(@sizeof(b) == 8)
    }

    // The boolean type
    {
        var a: bool = true
        var b: bool = false

        @assert(a == true)
        @assert(b == false)
        @assert(@sizeof(a) == 1)
        @assert(@sizeof(b) == 1)
    }

    // The string type. Strings are UTF8, and are stored as two 64 bits (pointer to value, and length)
    {
        var a: string = "string"
        @assert(a == "string")
        @assert(@sizeof(a) == 16)
    }

    // The character type use the string syntax, postfix with the type 'char'. It's a 32 bits (unicode 32)
    {
        var a: char = "C"'char
        @assert(a == "C"'char)
        @assert(@sizeof(a) == 4)
    }
}