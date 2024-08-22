/**
In Swag, strings are encoded in UTF-8, allowing them to support a wide range of characters from different languages and symbol sets.
*/

/**
Strings can be compared directly for equality.
*/
#test
{
    const a = "this is a Chinese character: 是"
    #assert a == "this is a Chinese character: 是"

    const b = "these are some Cyrillic characters: ӜИ"
    #assert b == "these are some Cyrillic characters: ӜИ"
}

/**
A `rune` in Swag represents a Unicode code point and is stored as a 32-bit value.
*/
#test
{
    const a: rune = `是`
    #assert a == `是`
    #assert @sizeof(a) == @sizeof(u32)  // Ensures that a `rune` is the same size as a 32-bit integer.
}

/**
> WARNING: You cannot index a string to directly retrieve a `rune`, except in ASCII strings. This is because Swag does not include the runtime cost of UTF-8 encoding/decoding in its string operations. However, the `Std.Core` module provides utilities to handle UTF-8 strings effectively.
*/

/**
When you index a string, you retrieve a byte rather than a `rune`.
*/
#test
{
    const a = "this is a Chinese character: 是"

    // Retrieves the first byte of the string.
    const b = a[0]
    #assert b == `t`
    #assert @typeof(b) == @typeof(u8)  // Confirms that the result is a byte (u8).

    // The 'X' character here does not have the index '1' because the preceding Chinese character
    // is encoded in UTF-8, which uses multiple bytes.
    const c = "是X是"
    #assert c[1] != `X` // False because byte 1 is not the character 'X'
}

/**
You can concatenate strings and other values at compile time using the `++` operator.
*/
#test
{
    const a = "the devil's number is " ++ 666
    #assert a == "the devil's number is 666"

    const b = 666
    let c   = "the devil's number is not " ++ (b + 1) ++ "!"
    @assert(c == "the devil's number is not 667!")

    let d = "there are " ++ 4 ++ " apples in " ++ (2 * 2) ++ " baskets"
    @assert(d == "there are 4 apples in 4 baskets")
}

/**
A string can be `null` if it has not been defined.
*/
#test
{
    var a: string
    @assert(a == null)  // Initially, 'a' is null.
    a = "not null"
    @assert(a != null)  // Now 'a' is not null.
    a = null
    @assert(a == null)  // 'a' is null again.
}

/**
# Character literals
A *character* is represented using backticks.
*/
#test
{
    let char0 = `a`
    let char1 = `我`
}

/**
By default, a character literal is a 32-bit integer that can be assigned to any integer type or to a `rune`, as long as the value fits within the target type.
*/
#test
{
    {
        let a: u8 = `a`
        let b: u16 = `a`
        let c: u32 = `我`
        let d: u64 = `我`
        let e: rune = `我`
    }

    {
        let a: s8 = `a`
        let b: s16 = `a`
        let c: s32 = `我`
        let d: s64 = `我`
    }
}

/**
However, you can specify the underlying type of a character literal by using a type suffix.
*/
#test
{
    let a = `0`'u8
    @assert(a == 48)              // ASCII value of '0' is 48.
    @assert(@typeof(a) == u8)

    let b = `1`'u16
    @assert(b == 49)              // ASCII value of '1' is 49.
    @assert(@typeof(b) == u16)

    let c = `2`'u32
    @assert(c == 50)              // ASCII value of '2' is 50.
    @assert(@typeof(c) == u32)

    let d = `3`'u64
    @assert(d == 51)              // ASCII value of '3' is 51.
    @assert(@typeof(d) == u64)

    let e = `4`'rune
    @assert(e == 52)              // ASCII value of '4' is 52.
    @assert(@typeof(e) == rune)
}

/**
# Escape sequences
Strings and character literals can contain *escape sequences* to represent special characters.
*/

/**
An escape sequence begins with a backslash `\`.
*/
#test
{
    const a = "this is ASCII code 0x00:   \0"     // null
    const b = "this is ASCII code 0x07:   \a"     // bell
    const c = "this is ASCII code 0x08:   \b"     // backspace
    const d = "this is ASCII code 0x09:   \t"     // horizontal tab
    const e = "this is ASCII code 0x0A:   \n"     // line feed
    const f = "this is ASCII code 0x0B:   \v"     // vertical tab
    const g = "this is ASCII code 0x0C:   \f"     // form feed
    const h = "this is ASCII code 0x0D:   \r"     // carriage return
    const i = "this is ASCII code 0x22:   \""     // double quote
    const j = "this is ASCII code 0x27:   \'"     // single quote
    const k = "this is ASCII code 0x60:   \`"     // backtick
    const l = "this is ASCII code 0x5C:   \\"     // backslash
}

/**
Escape sequences can also specify ASCII or Unicode values.
*/
#test
{
    {
        const a = "\x26"           // 1 byte, hexadecimal, extended ASCII
        const b = "\u2626"         // 2 bytes, hexadecimal, Unicode 16-bit
        const c = "\U00101234"     // 4 bytes, hexadecimal, Unicode 32-bit
    }

    {
        const d = "\u2F46\u2F46"
        #assert d == "⽆⽆"

        const e = `\u2F46`
        #assert e == `⽆`
    }
}

/**
# Raw strings
A *raw string* does not process escape sequences or special characters within it.
*/

/**
A raw string starts and ends with the `#` character.
*/
#test
{
    const a = #"\u2F46"#
    #assert a != "⽆"               // The raw string does not interpret \u2F46.
    #assert a == #"\u2F46"#         // The raw string matches the literal content.
}

/**
These two strings are equivalent:
*/
#test
{
    const a = "\\hello \\world"     // Using escape sequences
    const b = #"\hello \world"#     // As a raw string, no escaping needed
    #assert a == b                  // Both strings are identical.
}

/**
A raw string can span multiple lines because the line feed is part of the string.
*/
#test
{
    const a = #"this is
                a
                string
                "#
}

/**
In a multi-line raw string, all leading spaces before the closing `"#` are removed from each line. So, the above string is equivalent to:
*/
#test
{
    // this is
    // a
    // string
}

/**
# Multiline strings
A multiline string starts and ends with `"""`. Unlike raw strings, multiline strings still process escape sequences.
*/
#test
{
    const a = """this is
                 a
                 string
                 """

    // Equivalent to:

    // this is
    // a
    // string
}

/**
In a multiline or raw string, if you end a line with `\`, the following 'end of line' (EOL) will **not** be included in the string.
*/
#test
{
    const a = """\
              this is
              a
              string
              """
    // Is equivalent to:
    // this is
    // a
    // string
}

/**
# @stringof and @nameof
The `@stringof` intrinsic returns the string representation of a constant expression at compile time.
*/
#test
{
    const X = 1
    #assert @stringof(X) == "1"
    #assert @stringof(X + 10) == "11"
}

/**
You can use the `@nameof` intrinsic to return the name of a variable, function, etc., as a string.
*/
#test
{
    const X = 1
    #assert @nameof(X) == "X"
}
