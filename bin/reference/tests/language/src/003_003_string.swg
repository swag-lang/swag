/**
# UTF-8 Encoding
Swag uses UTF-8 encoding for strings, which allows representation of a wide range of characters,
including those from various languages and symbol sets.
*/

/**
# String Comparison
Strings can be compared directly for equality using the `==` operator.
*/
#test
{
    const a = "this is a Chinese character: 是" // String with a Chinese character
    #assert(a == "this is a Chinese character: 是")
    // Assert equality between two identical strings

    const b = "these are some Cyrillic characters: ӜИ" // String with Cyrillic characters
    #assert(b == "these are some Cyrillic characters: ӜИ")
    // Assert equality for another string
}

/**
# The 'rune' Type
A `rune` represents a Unicode code point and is stored as a 32-bit value, ensuring
it can accommodate any Unicode character.
*/
#test
{
    const a: rune = '是' // Define a rune using a Chinese character
    #assert(a == '是')
    // Assert the rune matches the expected value
    #assert(#sizeof(a) == #sizeof(u32))
    // Confirm the size of a rune is 32 bits (same as u32)
}

/**
> WARNING: Direct indexing of a string to retrieve a `rune` is not possible, except for ASCII strings.
> This is because Swag avoids the runtime overhead of UTF-8 decoding in string operations. However,
> the `Std.Core` module provides utilities for working with UTF-8 encoded strings.
*/

/**
# String Indexing
When indexing a string, Swag returns a byte (`u8`), not a `rune`, which reflects the underlying
UTF-8 encoding.
*/
#test
{
    const a = "this is a Chinese character: 是" // Define a string with mixed characters

    // Retrieves the first byte of the string, which corresponds to the character 't'
    const b = a[0]
    #assert(b == 't')
    // Assert the first byte is 't'
    #assert(#typeof(b) == #typeof(u8))
    // Confirm the type is `u8` (byte)

    // UTF-8 encoding affects indexing. 'X' is not at index 1 due to the multibyte encoding of the
    // preceding Chinese character.
    const c = "是X是"
    #assert(c[1] != 'X')
    // Assert that the byte at index 1 is not 'X'
}

/**
# String Concatenation
Swag allows compile-time concatenation of strings and other values using the `++` operator.
*/
#test
{
    const a = "the devil's number is " ++ 666 // Concatenation of a string with a number
    #assert(a == "the devil's number is 666")
    // Assert the resulting string is as expected

    const b = 666
    let c   = "the devil's number is not " ++ (b + 1) ++ "!"
    @assert(c == "the devil's number is not 667!") // Assert the concatenated string after arithmetic

    let d = "there are " ++ 4 ++ " apples in " ++ (2 * 2) ++ " baskets"
    @assert(d == "there are 4 apples in 4 baskets") // Concatenation with a mix of literals and expressions
}

/**
# Null Strings
A string can be `null` if it has not been initialized. This behavior can be used to
check whether a string has been assigned a value.
*/
#test
{
    var a: string // Declare a string variable without initialization
    @assert(a == null) // Initially, the string is null
    a = "not null" // Assign a value to the string
    @assert(a != null) // Assert that the string is no longer null
    a = null // Set the string back to null
    @assert(a == null) // Confirm the string is null again
}

/**
# Character Literals
Character literals are enclosed in quotes. These literals can represent any Unicode character, not just ASCII characters.

> NOTE:
> A character literal's quotation mark must be preceded by a symbol or a space to avoid confusion with a type suffix.
*/
#test
{
    let char0 = 'a'       // ASCII character literal
    let char1 = '我'     // Unicode character literal
    let value = 5's32     // This is a quote for a type suffix
}

/**
# Default Type of Character Literals
A character literal is a 32-bit integer by default. It can be assigned to any integer type or a `rune`, provided the value fits within the target type.
*/
#test
{
    {
        let a: u8 = 'a'         // Assign character literal to an 8-bit unsigned integer
        let b: u16 = 'a'        // Assign to a 16-bit unsigned integer
        let c: u32 = '我'      // Assign to a 32-bit unsigned integer
        let d: u64 = '我'      // Assign to a 64-bit unsigned integer
        let e: rune = '我'     // Assign to a rune (32-bit)
    }

    {
        let a: s8 = 'a'        // Assign to an 8-bit signed integer
        let b: s16 = 'a'       // Assign to a 16-bit signed integer
        let c: s32 = '我'     // Assign to a 32-bit signed integer
        let d: s64 = '我'     // Assign to a 64-bit signed integer
    }
}

/**
# Specifying Character Literal Types
Swag allows you to specify the type of a character literal using a suffix. This is useful for
controlling the storage size of the character.
*/
#test
{
    let a = '0''u8 // Character literal explicitly typed as u8
    @assert(a == 48) // ASCII value of '0' is 48
    @assert(#typeof(a) == u8) // Confirm type is u8

    let b = '1''u16 // Character literal explicitly typed as u16
    @assert(b == 49) // ASCII value of '1' is 49
    @assert(#typeof(b) == u16) // Confirm type is u16

    let c = '2''u32 // Character literal explicitly typed as u32
    @assert(c == 50) // ASCII value of '2' is 50
    @assert(#typeof(c) == u32) // Confirm type is u32

    let d = '3''u64 // Character literal explicitly typed as u64
    @assert(d == 51) // ASCII value of '3' is 51
    @assert(#typeof(d) == u64) // Confirm type is u64

    let e = '4''rune // Character literal explicitly typed as rune
    @assert(e == 52) // ASCII value of '4' is 52
    @assert(#typeof(e) == rune) // Confirm type is rune
}

/**
# Escape Sequences
Swag supports escape sequences in strings and character literals, allowing special characters
to be represented within these literals.
*/

/**
An escape sequence starts with a backslash `\` and is followed by a specific character.
*/
#test
{
    const a = "this is ASCII code 0x00:   \0"     // Null character escape sequence
    const b = "this is ASCII code 0x07:   \a"     // Bell (alert) escape sequence
    const c = "this is ASCII code 0x08:   \b"     // Backspace escape sequence
    const d = "this is ASCII code 0x09:   \t"     // Horizontal tab escape sequence
    const e = "this is ASCII code 0x0A:   \n"     // Line feed escape sequence
    const f = "this is ASCII code 0x0B:   \v"     // Vertical tab escape sequence
    const g = "this is ASCII code 0x0C:   \f"     // Form feed escape sequence
    const h = "this is ASCII code 0x0D:   \r"     // Carriage return escape sequence
    const i = "this is ASCII code 0x22:   \""     // Double quote escape sequence
    const j = "this is ASCII code 0x27:   \'"     // Single quote escape sequence
    const k = "this is ASCII code 0x5C:   \\"     // Backslash escape sequence
}

/**
# ASCII and Unicode Escape Sequences
Escape sequences can also be used to specify characters via their ASCII or Unicode values.
*/
#test
{
    {
        const a = "\x26"           // 1-byte hexadecimal ASCII escape sequence
        const b = "\u2626"         // 2-byte hexadecimal Unicode 16-bit escape sequence
        const c = "\U00101234"     // 4-byte hexadecimal Unicode 32-bit escape sequence
    }

    {
        const d = "\u2F46\u2F46" // Unicode escape sequences for two identical characters
        #assert(d == "⽆⽆")
        // Assert the resulting string matches expected characters

        const e = '\u2F46' // Unicode escape sequence as a rune literal
        #assert(e == '⽆')
        // Assert the rune matches the expected character
    }
}

/**
# Raw Strings
A raw string is a string where escape sequences and special characters are not processed.
This can be useful for strings that contain many special characters or backslashes.
*/

/**
A raw string is enclosed within `#` characters, which ensures that its content is taken literally.
*/
#test
{
    const a = #"\u2F46"# // Raw string containing a Unicode escape sequence
    #assert(a != "⽆")
    // Raw string does not interpret the escape sequence
    #assert(a == #"\u2F46"#)
    // Raw string content matches the literal input
}

/**
These two strings are equivalent, even though one uses escape sequences and the other is raw:
*/
#test
{
    const a = "\\hello \\world"     // String with escape sequences
    const b = #"\hello \world"#     // Equivalent raw string
    #assert(a == b)
    // Assert both strings are identical
}

/**
# Multiline Raw Strings
Raw strings can span multiple lines, and all leading spaces before the closing `"#` are
removed from each line.
*/
#test
{
    const a = #"this is
                a
                string
                "#
}

/**
In the above example, the multiline raw string retains its formatting, except that leading spaces
before the closing `"#` are stripped from each line.
*/
#test
{
    // The resulting string is:
    // this is
    // a
    // string
}

/**
# Multiline Strings
Multiline strings start and end with `"""`. Unlike raw strings, they still process escape
sequences.
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
In a multiline or raw string, ending a line with `\` prevents the following end of line (EOL)
from being included in the string.
*/
#test
{
    const a = """\
              this is
              a
              string
              """
    // The resulting string is:
    // this is
    // a
    // string
}

/**
# #stringof and #nameof Intrinsics
The `#stringof` intrinsic returns the string representation of a constant expression
at compile time.
*/
#test
{
    const X = 1
    #assert(#stringof(X) == "1")
    // The string representation of X is "1"
    #assert(#stringof(X + 10) == "11")
    // Expression evaluation is also possible
}

/**
The `#nameof` intrinsic returns the name of a variable, function, etc., as a string.
*/
#test
{
    const X = 1
    #assert(#nameof(X) == "X")
    // Returns the variable name "X"
}
