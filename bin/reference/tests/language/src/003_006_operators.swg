/**
# Arithmetic Operators

Arithmetic operators perform basic mathematical operations such as addition,
subtraction, multiplication, division, and modulus (remainder).
*/
#test
{
    var x: s32 = 10

    // Addition
    x = x + 1

    // Subtraction
    x = x - 1

    // Multiplication
    x = x * 2

    // Division
    x = x / 2

    // Modulus
    x = x % 2
}

/**
# Bitwise Operators

Bitwise operators manipulate individual bits of integral types. They include
bitwise AND, OR, XOR, and bit shifting.
*/
#test
{
    var x: s32 = 10

    x = x ^ 2 // XOR
    x = x & 0b0000_0001's32 // AND
    x = x | cast(s32) 0b0001 // OR
    x = x << 1 // Shift left
    x = x >> 1 // Shift right
}

/**
# Assignment Operators

Assignment operators perform an operation and immediately assign the result to
the left operand, creating concise expressions.
*/
#test
{
    var x: s32 = 10

    x += 1
    x -= 1
    x *= 2
    x /= 2
    x %= 2
    x ^= 2
    x |= 0b0001
    x &= 0b0001
    x <<= 1
    x >>= 1
}

/**
# Unary Operators

Unary operators operate on a single operand. These include logical NOT, bitwise
NOT, and negation.
*/
#test
{
    var x = true
    var y = 0b0000_0001'u8
    var z = 1

    x = !x // Logical NOT
    y = ~y // Bitwise NOT
    z = -z // Negation

    @assert(z == -1)
    @assert(x == false)
    @assert(y == 0b1111_1110)
}

/**
# Comparison Operators

Comparison operators compare two values and return a boolean result. They include
equality, inequality, and relational comparisons.
*/
#test
{
    {
        var a = false
        a = 1 == 1 ? true : false
        a = 1 != 1 ? true : false
        a = 1 <= 1 ? true : false
        a = 1 >= 1 ? true : false
        a = 1 < 1 ? true : false
        a = 1 > 1 ? true : false
    }

    {
        let x = 5
        let y = 10
        @assert(x == 5)
        @assert(x != 10)
        @assert(x <= 5)
        @assert(x < 10)
        @assert(x >= 5)
        @assert(x > 0)
    }
}

/**
# Logical Operators

Logical operators evaluate expressions and return a boolean result. Swag uses
'and' and 'or' instead of '&&' and '||' from C/C++.
*/
#test
{
    var a = false
    a = (1 > 10) and (10 < 1)
    a = (1 > 10) or (10 < 1)
}

/**
# Ternary Operator

The ternary operator tests an expression and returns one of two values depending
on whether the condition is true or false.

Syntax:
`A = Condition ? B : C`
*/
#test
{
    let x = true ? 1 : 666
    @assert(x == 1)

    let y = (x == 52) ? 1 : 666
    @assert(y == 666)
}

/**
# Spaceship Operator

The '<=>' operator returns -1, 0, or 1 depending on whether the left operand is
less than, equal to, or greater than the right operand. The result type is 's32'.

```swag
A <=> B == -1 if A < B
A <=> B ==  0 if A == B
A <=> B ==  1 if A > B
````

*/
#test
{
    {
        let a = -1.5 <=> 2.31
        #assert(#typeof(a) == s32)
        @assert(a == -1)

        @assert(-10 <=> 10 == -1)
        @assert(10 <=> -10 == 1)
        @assert(10 <=> 10 == 0)
    }

    {
        let x1 = 10 <=> 20
        @assert(x1 == -1)

        let x2 = 20 <=> 10
        @assert(x2 == 1)

        let x3 = 20 <=> 20
        @assert(x3 == 0)
    }
}

/**

# Null-Coalescing Operator

The 'orelse' operator returns the left-hand expression if it is not null;
otherwise, it returns the right-hand expression.
*/
#test
{
    var a = "string1"
    let b = "string2"

    var c = a orelse b
    @assert(c == "string1")

    a = null
    c = a orelse b
    @assert(c == "string2")
}

/**
The 'orelse' operator also works with numeric and basic types.
*/
#test
{
    let a = 0
    let b = 1
    let c = a orelse b
    @assert(c == b)
}

/**

# Type Promotion

Unlike C, Swag does not automatically promote small integer types (8-bit or
16-bit) to 32 bits. However, when operands differ in type, promotion occurs to
match the larger or signed type.
*/
#test
{
    #assert(#typeof(0'u8 + 1'u8) == u8)
    #assert(#typeof(0'u8 + 1'u16) == u16)
    #assert(#typeof(0'u8 + 1'u32) == u32)
    #assert(#typeof(0'u8 + 1'u64) == u64)
    #assert(#typeof(0'u8 + 1's8) == s8)
    #assert(#typeof(0'u8 + 1's16) == s16)
    #assert(#typeof(0'u8 + 1's32) == s32)
    #assert(#typeof(0'u8 + 1's64) == s64)
    #assert(#typeof(0'u8 + 1'f32) == f32)
    #assert(#typeof(0'u8 + 1'f64) == f64)
    #assert(#typeof(0's8 + 1'u16) == u16)
}

/**
To prevent overflow during small integer operations, you can apply the '#prom'
modifier to promote operands to at least 32 bits before the operation.
*/
#test
{
    #assert(#typeof(255'u8 + #prom 1'u8) == u32)
    #assert(255'u8 + #prom 1'u8 == 256)
}

/**

# Operator Precedence

```swag
~
* / %
+ -
>> <<
&
|
^
<=>
== !=
< <= > >=
and
or
```

When two operators share the same precedence level, expressions are evaluated
from left to right.
*/
#test
{
    @assert(10 + 2 * 3 == 16)
    @assert((10 + 2) * 3 == 36)
    @assert((5 + 3 < 10 - 2) == false)
    @assert((false and false or true) == true)
    @assert((10 & 2 << 1) == 0)
    @assert(((10 & 2) << 1) == 4)
}
