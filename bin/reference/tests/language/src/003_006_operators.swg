/**
# Arithmetic Operators
These operators perform basic arithmetic operations such as addition, subtraction, multiplication,
and division. Modulus operation is also supported to get the remainder of a division.
*/
#test
{
    var x: s32 = 10

    // Addition: Increments x by 1
    x = x + 1                                                              

    // Subtraction: Decrements x by 1
    x = x - 1                                                              

    // Multiplication: Multiplies x by 2
    x = x * 2                                                              

    // Division: Divides x by 2
    x = x / 2                                                              

    // Modulus: Finds remainder when x is divided by 2
    x = x % 2                                                              
}

/**
# Bitwise Operators
Bitwise operators manipulate individual bits of integral types. These operators perform operations 
such as XOR, AND, OR, and bit shifting.
*/
#test
{
    var x: s32 = 10

    // XOR: Bitwise XOR with 2
    x = x ^ 2                                                              

    // Bitwise AND: Applies a bitmask to retain only the least significant bit
    x = x & 0b0000_0001's32                                                

    // Bitwise OR: Sets the least significant bit to 1
    x = x | cast(s32) 0b0000_0001                                          

    // Shift Left: Shifts bits to the left by 1 position
    x = x << 1                                                             

    // Shift Right: Shifts bits to the right by 1 position
    x = x >> 1                                                             
}

/**
# Assignment Operators
These operators perform the basic arithmetic or bitwise operation and assign the result directly 
to the left operand. The 'affect' versions of these operators make the code concise and clear.
*/
#test
{
    var x: s32 = 10

    x += 1           // Addition assignment
    x -= 1           // Subtraction assignment
    x *= 2           // Multiplication assignment
    x /= 2           // Division assignment
    x %= 2           // Modulus assignment
    x ^= 2           // XOR assignment
    x |= 0b0000_0001 // Bitwise OR assignment
    x &= 0b0000_0001 // Bitwise AND assignment
    x <<= 1          // Shift left assignment
    x >>= 1          // Shift right assignment
}

/**
# Unary Operators
Unary operators operate on a single operand. These include logical NOT, bitwise NOT, and negation.
*/
#test
{
    var x = true
    var y = 0b0000_0001'u8
    var z = 1

    // Invert boolean: Logical NOT
    x = !x                                                               

    // Invert bits: Bitwise NOT
    y = ~y                                                               

    // Negation: Unary minus
    z = -z                                                               

    @assert(z == -1)
    @assert(x == false)
    @assert(y == 0b1111_1110)
}

/**
# Comparison Operators
Comparison operators compare two values and return a boolean result. They include equality, inequality,
and relational operators.
*/
#test
{
    {
        var a = false

        // Equal: Checks if two values are equal
        a = 1 == 1 ? true : false                                        

        // Not equal: Checks if two values are not equal
        a = 1 != 1 ? true : false                                        

        // Less than or equal: Checks if the left value is less than or equal to the right value
        a = 1 <= 1 ? true : false                                        

        // Greater than or equal: Checks if the left value is greater than or equal to the right value
        a = 1 >= 1 ? true : false                                        

        // Less than: Checks if the left value is less than the right value
        a = 1 < 1 ? true : false                                         

        // Greater than: Checks if the left value is greater than the right value
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
Logical operators evaluate expressions to return a boolean value. Swag uses `and` and `or` instead 
of `&&` and `||` found in C/C++.
*/
#test
{
    var a = false
    a = (1 > 10) and (10 < 1)  // Logical AND
    a = (1 > 10) or (10 < 1)   // Logical OR
}

/**
# Ternary Operator
The ternary operator tests an expression and returns one of two values depending on the result of 
the test. The syntax is `A = Expression ? B : C`, where `B` is returned if the expression is true,
and `C` is returned if the expression is false.
*/
#test
{
    // Returns 1 because the expression 'true' is... true.
    let x = true ? 1 : 666
    @assert(x == 1)

    // Returns 666 because the expression 'x == 52' is false.
    let y = (x == 52) ? 1 : 666
    @assert(y == 666)
}

/**
# Spaceshift Operator
Operator `<=>` returns -1, 0, or 1 if the left expression is lower, equal, or greater than the right 
expression, respectively. The returned type is `s32`.
```swag
A <=> B == -1 if A < B
A <=> B == 0  if A == B
A <=> B == 1  if A > B
  ```
*/
#test
{
    {
        let a = -1.5 <=> 2.31
        #assert @typeof(a) == s32
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
The operator `orelse` returns the left expression if it is not null, otherwise it returns the right 
expression.
*/

/**
Works with strings, pointers, and structures with the `opData` special function (covered later).
*/
#test
{
    var a = "string1"
    let b = "string2"

    // c = a if a is not null, else c = b.
    var c = a orelse b
    @assert(c == "string1")

    a = null
    c = a orelse b
    @assert(c == "string2")
}

/**
Works also for basic types like integers.
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
Unlike C, types are not promoted to 32 bits when dealing with 8 or 16-bit types. However, types will 
be promoted if the two sides of an operation do not have the same type.
*/
#test
{
    #assert @typeof(0'u8 + 1'u8) == u8
    #assert @typeof(0'u8 + 1'u16) == u16 // Priority to bigger type
    #assert @typeof(0'u8 + 1'u32) == u32
    #assert @typeof(0'u8 + 1'u64) == u64
    #assert @typeof(0'u8 + 1's8) == s8   // Priority to signed type
    #assert @typeof(0'u8 + 1's16) == s16
    #assert @typeof(0'u8 + 1's32) == s32
    #assert @typeof(0'u8 + 1's64) == s64
    #assert @typeof(0'u8 + 1'f32) == f32
    #assert @typeof(0'u8 + 1'f64) == f64

    #assert @typeof(0's8 + 1'u16) == u16 // Priority to bigger type also
}

/**
This means that an 8/16-bit operation (like an addition) can more easily overflow if you do not take 
care. In that case, you can use the ` #prom` modifier on the operation, which will promote the type 
to at least 32 bits. The operation will be done accordingly.
*/
#test
{
    #assert @typeof(255'u8 + #prom 1'u8) == u32
    #assert 255'u8 + #prom 1'u8 == 256 // No overflow, because the operation is done in 32 bits.
}

/**
We'll see later how Swag deals with that kind of overflow, and more generally, with **safety**.
*/

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
If two operators have the same precedence, the expression is evaluated from left to right.
*/
#test
{
    // Multiplication before addition
    @assert(10 + 2 * 3 == 16)
    // Parentheses change precedence
    @assert((10 + 2) * 3 == 36)
    // Addition and subtraction before comparison
    @assert((5 + 3 < 10 - 2) == false)
    // 'and' before 'or'
    @assert((false and false or true) == true)
    // '<<' before '&'
    @assert((10 & 2 << 1) == 0)
    @assert(((10 & 2) << 1) == 4)
}
