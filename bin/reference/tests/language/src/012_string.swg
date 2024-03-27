/**
In Swag, strings are encoded in UTF8.
*/

/**
They also can be compared.
*/
#test
{
    const a = "this is a chinese character: 是"
    #assert a == "this is a chinese character: 是"

    const b = "these are some cyrillic characters: ӜИ"
    #assert b == "these are some cyrillic characters: ӜИ"
}

/**
A rune is an unicode codepoint, and is 32 bits.
*/
#test
{
    const a: rune = `是`
    #assert a == `是`
    #assert @sizeof(a) == @sizeof(u32)
}

/**
> WARNING: You cannot index a string to get a rune, except in ascii strings. This is because we didn't want the runtime to come with the cost of UTF8 encoding/decoding. But note that the `Std.Core` module will have all you need to manipulate UTF8 strings.
*/

/**
So in that case you will retrieve a byte.
*/
#test
{
    const a = "this is a chinese character: 是"

    // Get the first byte of the string
    const b = a[0]
    #assert b == `t`
    #assert @typeof(b) == @typeof(u8)

    // Here, the 'X' character in the middle does not have the index '1', because the
    // chinese character before is encoded in UTF8 with more than 1 byte.
    const c = "是X是"
    #assert c[1] != `X` // False because the byte number 1 is not the character 'X'
}

/**
You can concatenate some values if the values are known at compile time. Use the `++` operator for that.
*/
#test
{
    const a = "the devil number is " ++ 666
    #assert a == "the devil number is 666"

    const b = 666
    let c   = "the devil number is not " ++ (b + 1) ++ "!"
    @assert(c == "the devil number is not 667!")

    let d = "they are " ++ 4 ++ " apples in " ++ (2 * 2) ++ " baskets"
    @assert(d == "they are 4 apples in 4 baskets")
}

/**
A string can be `null` if not defined.
*/
#test
{
    var a: string
    @assert(a == null)
    a = "null"
    @assert(a != null)
    a = null
    @assert(a == null)
}

/**
# Character literals
A *character* is enclosed with **backticks**.
*/
#test
{
    let char0 = `a`
    let char1 = `我`
}

/**
By default, it's a lazy 32 bits integer that can be assigned to all integers (as long as it fits) and to the type `rune`.
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
But the underlying type of a character can be forced with the use of a type postfix.
*/
#test
{
    let a = `0`'u8
    @assert(a == 48)
    @assert(@typeof(a) == u8)

    let b = `1`'u16
    @assert(b == 49)
    @assert(@typeof(b) == u16)

    let c = `2`'u32
    @assert(c == 50)
    @assert(@typeof(c) == u32)

    let d = `3`'u64
    @assert(d == 51)
    @assert(@typeof(d) == u64)

    let e = `4`'rune
    @assert(e == 52)
    @assert(@typeof(e) == rune)
}

/**
# Escape sequences
A string and a character can contain some *escape sequences* to specify special characters.
*/

/**
An escape sequence starts with a backslash `\\`.
*/
#test
{
    const a = "this is code ascii 0x00:   \0"     // null
    const b = "this is code ascii 0x07:   \a"     // bell
    const c = "this is code ascii 0x08:   \b"     // backspace
    const d = "this is code ascii 0x09:   \t"     // horizontal tab
    const e = "this is code ascii 0x0A:   \n"     // line feed
    const f = "this is code ascii 0x0B:   \v"     // vertical tab
    const g = "this is code ascii 0x0C:   \f"     // form feed
    const h = "this is code ascii 0x0D:   \r"     // carriage return
    const i = "this is code ascii 0x22:   \""     // double quote
    const j = "this is code ascii 0x27:   \'"     // single quote
    const k = "this is code ascii 0x60:   \`"     // backtick
    const l = "this is code ascii 0x5C:   \\"     // backslash
}

/**
An escape sequence can also be used to specify an ascii or a unicode value.
*/
#test
{
    {
        const a = "\x26"           // 1 byte, hexadecimal, extended ascii
        const b = "\u2626"         // 2 bytes, hexadecimal, unicode 16 bits
        const c = "\U00101234"     // 4 bytes, hexadecimal, unicode 32 bits
    }

    {
        const d = "\u2F46\u2F46"
        #assert d == "⽆⽆"

        const e = `\u2F46`
        #assert e == `⽆`
    }
}

/**
# Raw string
A *raw string* does not transform the escape sequences inside it.
*/

/**
A raw string starts and ends with the character `#`.
*/
#test
{
    const a = #"\u2F46"#
    #assert a != "⽆"
    #assert a == #"\u2F46"#
}

/**
This are equivalent:
*/
#test
{
    const a = "\\hello \\world"     // By using an escape character
    const b = #"\hello \world"#
    // Without, because they are not transformed
    #assert a == b
}

/**
A raw string can spawn on multiple lines because the line feed is now part of the string.
*/
#test
{
    const a = #"this is
a
string
"#
}

/**
Every blanks **before** the ending mark `"@` will be removed from every other lines, so the string before is equivalent to :
*/
#test
{
}

/**
# Multiline string
A multiline string starts and ends with `"""`. Unlike raw strings, they still evaluate escape sequences.
*/
#test
{
    const a = """this is
a
string
"""
}

// Equivalent to :

/**
In a multiline or a raw string, if you end a line with `\\`, the following 'eol' will **not** be part of the string.
*/
#test
{
    const a = """this is
a
string
"""
}

/**
# @stringof and @nameof
You can use the instrinsic `@stringof` to return at compile time the result of a constant expression as a string.
*/
#test
{
    const X = 1
    #assert @stringof(X) == "1"
    #assert @stringof(X + 10) == "11"
}

/**
You can also use `@nameof` to return the name of a variable, function etc.
*/
#test
{
    const X = 1
    #assert @nameof(X) == "X"
}
