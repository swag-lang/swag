/**
# UTF-8 Encoding

Swag uses UTF-8 encoding for strings, allowing representation of characters from
virtually all languages and symbol sets.
*/

/**
# String Comparison

Strings can be compared directly for equality using the '==' operator.
*/
#test
{
    const a = "this is a Chinese character: 是"
    #assert(a == "this is a Chinese character: 是")

    const b = "these are some Cyrillic characters: ӜИ"
    #assert(b == "these are some Cyrillic characters: ӜИ")
}

/**
# The 'rune' Type

A 'rune' represents a Unicode code point and is stored as a 32-bit value,
ensuring it can accommodate any Unicode character.
*/
#test
{
    const a: rune = '是'
    #assert(a == '是')
    #assert(#sizeof(a) == #sizeof(u32))
}

/**
> WARNING:
> Direct indexing of a string to retrieve a 'rune' is not supported, except for
> ASCII strings. Swag avoids the runtime overhead of UTF-8 decoding.
> The 'Std.Core' module provides utilities for working with UTF-8 strings.
*/

/**
# String Indexing

When indexing a string, Swag returns a byte ('u8'), not a 'rune'. This reflects
the underlying UTF-8 encoding.
*/
#test
{
    const a = "this is a Chinese character: 是"

    // Retrieve the first byte ('t')
    const b = a[0]
    #assert(b == 't')
    #assert(#typeof(b) == #typeof(u8))

    // Multibyte UTF-8 affects indexing
    const c = "是X是"
    #assert(c[1] != 'X')
}

/**
# String Concatenation

Swag allows compile-time concatenation of strings and other values using the
'++' operator.
*/
#test
{
    const a = "the devil's number is " ++ 666
    #assert(a == "the devil's number is 666")

    const b = 666
    let c   = "the devil's number is not " ++ (b + 1) ++ "!"
    @assert(c == "the devil's number is not 667!")

    let d = "there are " ++ 4 ++ " apples in " ++ (2 * 2) ++ " baskets"
    @assert(d == "there are 4 apples in 4 baskets")
}

/**
# Null Strings

A string can be 'null' if it has not been initialized. This allows checking
whether a string has been assigned a value.
*/
#test
{
    var a: string
    @assert(a == null)
    a = "not null"
    @assert(a != null)
    a = null
    @assert(a == null)
}

/**
# Character Literals

Character literals are enclosed in quotes and can represent any Unicode character,
not just ASCII.

> NOTE:
> A character literal's quote must follow a symbol or space to avoid confusion
> with a type suffix.
*/
#test
{
    let char0 = 'a'
    let char1 = '我'
    let value = 5's32     // Type suffix example
}

/**
# Default Type of Character Literals

A character literal is a 32-bit integer by default. It can be assigned to any
integer type or a 'rune' if the value fits within the target type.
*/
#test
{
    {
        let a: u8 = 'a'
        let b: u16 = 'a'
        let c: u32 = '我'
        let d: u64 = '我'
        let e: rune = '我'
    }

    {
        let a: s8 = 'a'
        let b: s16 = 'a'
        let c: s32 = '我'
        let d: s64 = '我'
    }
}

/**
# Specifying Character Literal Types

You can specify a character literal’s type using a suffix to control its storage size.
*/
#test
{
    let a = '0''u8
    @assert(a == 48)
    @assert(#typeof(a) == u8)

    let b = '1''u16
    @assert(b == 49)
    @assert(#typeof(b) == u16)

    let c = '2''u32
    @assert(c == 50)
    @assert(#typeof(c) == u32)

    let d = '3''u64
    @assert(d == 51)
    @assert(#typeof(d) == u64)

    let e = '4''rune
    @assert(e == 52)
    @assert(#typeof(e) == rune)
}

/**
# Escape Sequences

Swag supports escape sequences in strings and character literals to represent
special characters.
*/

/**
An escape sequence starts with a backslash ('\') followed by a specific character.
*/
#test
{
    const a = "this is ASCII code 0x00:   \0"
    const b = "this is ASCII code 0x07:   \a"
    const c = "this is ASCII code 0x08:   \b"
    const d = "this is ASCII code 0x09:   \t"
    const e = "this is ASCII code 0x0A:   \n"
    const f = "this is ASCII code 0x0B:   \v"
    const g = "this is ASCII code 0x0C:   \f"
    const h = "this is ASCII code 0x0D:   \r"
    const i = "this is ASCII code 0x22:   \""
    const j = "this is ASCII code 0x27:   \'"
    const k = "this is ASCII code 0x5C:   \\"
}

/**
# ASCII and Unicode Escape Sequences

Escape sequences can represent characters via their ASCII or Unicode values.
*/
#test
{
    {
        const a = "\x26"
        const b = "\u2626"
        const c = "\U00101234"
    }

    {
        const d = "\u2F46\u2F46"
        #assert(d == "⽆⽆")

        const e = '\u2F46'
        #assert(e == '⽆')
    }
}

/**
# Raw Strings

A raw string is one where escape sequences and special characters are not
processed. This is useful when working with many backslashes or special symbols.
*/

/**
A raw string is enclosed within '#' characters. Its content is taken literally.
*/
#test
{
    const a = #"\u2F46"# // Raw string containing a Unicode escape
    #assert(a != "⽆")
    #assert(a == #"\u2F46"#)
}

/**
Equivalent strings can be written using escape sequences or raw notation.
*/
#test
{
    const a = "\\hello \\world"
    const b = #"\hello \world"#
    #assert(a == b)
}

/**
# Multiline Raw Strings

Raw strings can span multiple lines. All leading spaces before the closing '"#'
are removed from each line.
*/
#test
{
    const a = #"this is
                a
                string
                "#
}

/**
# Multiline Strings

Multiline strings start and end with '"""'. Unlike raw strings, escape sequences
are still processed.
*/
#test
{
    const a = """this is
                 a
                 string
                 """
}

/**
In multiline or raw strings, ending a line with '\' prevents the following
end-of-line from being included.
*/
#test
{
    const a = """\
              this is
              a
              string
              """
}

/**
# '#stringof' and '#nameof' Intrinsics

The '#stringof' intrinsic returns the string representation of a constant
expression at compile time.
*/
#test
{
    const X = 1
    #assert(#stringof(X) == "1")
    #assert(#stringof(X + 10) == "11")
}

/**
The '#nameof' intrinsic returns the name of a variable, function, etc., as a string.
*/
#test
{
    const X = 1
    #assert(#nameof(X) == "X")
}
