#test
{
    // Basic binary operators
    {
        var x: s32 = 10

        x = x + 1
        x = x - 1
        x = x * 2
        x = x / 2
        x = x % 2 // Modulo
        x = x ^ 2 // Xor
        x = x & 0b0000_0001's32 // Bitmask and
        x = x | cast(s32) 0b0000_0001 // Bitmask or
        x = x << 1 // Shift bits left
        x = x >> 1 // Shift bits right
    }

    // Affect operators
    {
        var x: s32 = 10

        x += 1
        x -= 1
        x *= 2
        x /= 2
        x %= 2
        x ^= 2
        x |= 0b0000_0001's32
        x &= cast(s32) 0b0000_0001
        x <<= 1
        x >>= 1
    }

    // Unary operator
    {
        x := true
        x = !x      // Invert boolean
        @assert(x == false)

        y := 0b0000_0001'u8
        y = ~y      // Invert bits
        @assert(y == 0b1111_1110)

        z := 1
        z = -z      // Negative
        @assert(z == -1)
    }

    // Ternary operator
    {
        var x = true ? 1 : 666
        @assert(x == 1)
        var y = (x == 1) ? 1 : 666
        @assert(y == 1)
    }

    // Comparison operators
    {
        a := false

        a = 1 == 1 ? true : false
        a = 1 != 1 ? true : false
        a = 1 <= 1 ? true : false
        a = 1 >= 1 ? true : false
        a = 1 < 1 ? true : false
        a = 1 > 1 ? true : false
    }

    // Two ways comparison operator, which returns a s32 of -1, 0 or 1
    // A <=> B == -1 if A < B
    // A <=> B == 0  if A == B
    // A <=> B == 1  if A > B
    {
        a := -1.5 <=> 2.31
        @assert(@typeof(a) == s32)
        @assert((-1 <=> 1) == -1)
        @assert((1 <=> -1) == 1)
        @assert((1 <=> 1) == 0)
    }

    // Logic operator
    {
        a := false

        a = (1 > 10) && (10 < 1)    // And
        a = (1 > 10) || (10 < 1)    // Or
    }

    // Null conditional operator
    // Works with strings, pointers and structure with the 'opData' operator
    {
        a := "string1"
        b := "string2"
        c := a ?? b
        @assert(c == "string1")

        a = null
        c = a ?? b
        @assert(c == "string2")
    }
}