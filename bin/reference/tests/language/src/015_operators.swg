/**
These are the basic operators that can be used to manipulate values.
*/

#test
{
    // # Binary operators
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

    // # Affect operators
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

    // # Unary operators
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

    // # Comparison operators
    {
        a := false

        a = 1 == 1 ? true : false
        a = 1 != 1 ? true : false
        a = 1 <= 1 ? true : false
        a = 1 >= 1 ? true : false
        a = 1 < 1 ? true : false
        a = 1 > 1 ? true : false
    }

    {
        x := 5
        y := 10
        @assert(x == 5)
        @assert(x != 10)
        @assert(x <= 5)
        @assert(x < 10)
        @assert(x >= 5)
        @assert(x > 0)
    }
}

/**
# Logical operators
This is **not** `&&` and `||` like in C/C++.
*/
#test
{
    a := false
    a = (1 > 10) and (10 < 1)
    a = (1 > 10) or (10 < 1)
}

/**
# Ternary operator
The ternary operator will test an expression, and will return a value depending on the result of the test.
`A = Expression ? B : C` will return `B` if the expression is true, and will return `C` if the expression is false.
*/
#test
{
    // Returns 1 because the expression 'true' is... true.
    var x = true ? 1 : 666
    @assert(x == 1)

    // Returns 666 because the expression 'x == 52' is false.
    var y = (x == 52) ? 1 : 666
    @assert(y == 666)
}

/**
# Spaceshift operator
It will return a `s32` of -1, 0 or 1.
```
A <=> B == -1 if A < B
A <=> B == 0  if A == B
A <=> B == 1  if A > B
```
*/
#test
{
    {
        a := -1.5 <=> 2.31
        #assert @typeof(a) == s32
        @assert(a == -1)

        @assert(-10 <=>  10 == -1)
        @assert( 10 <=> -10 == 1)
        @assert( 10 <=>  10 == 0)
    }

    {
        x1 := 10 <=> 20
        @assert(x1 == -1)
        x2 := 20 <=> 10
        @assert(x2 == 1)
        x3 := 20 <=> 20
        @assert(x3 == 0)
    }
}

/**
# Null-coalescing operator
The operator `orelse` will return the left expression if it is not zero, otherwise it will return the right expression.
*/
#test
{
    // Works with strings, pointers and structures with the `opData` special function (we'll see that later).
    {
        a := "string1"
        b := "string2"

        // c = a if a is not null, else c = b.
        c := a orelse b
        @assert(c == "string1")

        a = null
        c = a orelse b
        @assert(c == "string2")
    }

    // Works also for basic types like integers.
    {
        a := 0
        b := 1
        c := a orelse b
        @assert(c == b)
    }
}

/**
# Type promotion
Unlike C, types are not promoted to 32 bits when dealing with 8 or 16 bits types. But types will be promoted if the two sides of an operation do not have the same type.
*/
#test
{
    #assert @typeof(0'u8 + 1'u8)  == u8
    #assert @typeof(0'u8 + 1'u16) == u16    // Priority to bigger type
    #assert @typeof(0'u8 + 1'u32) == u32
    #assert @typeof(0'u8 + 1'u64) == u64
    #assert @typeof(0'u8 + 1's8)  == s8     // Priority to signed type
    #assert @typeof(0'u8 + 1's16) == s16
    #assert @typeof(0'u8 + 1's32) == s32
    #assert @typeof(0'u8 + 1's64) == s64
    #assert @typeof(0'u8 + 1'f32) == f32
    #assert @typeof(0'u8 + 1'f64) == f64

    #assert @typeof(0's8 + 1'u16) == u16   // Priority to bigger type also

    // This means that a 8/16 bits operation (like an addition) can more easily overflow if you do not take care.
    // In that case, you can use the ',up' modifier on the operation, which will promote the type to at least 32 bits. The operation will be done accordingly.
    #assert @typeof(255'u8 +,up 1'u8) == u32
    #assert 255'u8 +,up 1'u8 == 256 // No overflow, because the operation is done in 32 bits.
}