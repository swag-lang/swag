/**
In Swag, strings are encoded in UTF8.
*/

#test
{
    // They also can be compared.
    {
        const a = "this is a chinese character: 是"
        #assert a == "this is a chinese character: 是"

        const b = "this are some cyrillic characters: ӜИ"
        #assert b == "this are some cyrillic characters: ӜИ"
    }

    // A rune is an unicode codepoint, and is 32 bits.
    {
        const a = "是"'rune
        #assert a == "是"'rune
        #assert @sizeof(a) == @sizeof(u32)
    }
}

/**
You cannot index a string to get a rune, except in ascii strings. This is because we didn't want the runtime to come with the cost of UTF8 encoding/decoding. But note that the `Std.Core` module will have all you need to manipulate UTF8 strings.
*/
#test
{
    // So in that case you will retrieve a byte.
    {
        const a = "this is a chinese character: 是"

        // Get the first byte of the string
        const b = a[0]
        #assert b == "t"'u8
        #assert @typeof(b) == @typeof(u8)

        // Here, the 'X' character in the middle does not have the index '1', because the chinese character before is
        // encoded in UTF8 with more than 1 byte.
        const c = "是X是"
        #assert c[1] != "X"'u8 // False because the byte number 1 is not the character 'X'
    }
}

/**
Multiple adjacent strings are compiled as one.
*/
#test
{
    const a = "this is "   "a"   " string"
    #assert a == "this is a string"
}

/**
You can concatenate some values if the values are known at compile time. Use the `++` operator for that.
*/
#test
{
    const a = "the devil number is " ++ 666
    #assert a == "the devil number is 666"

    const b = 666
    var c = "the devil number is not " ++ (b + 1) ++ "!"
    @assert(c == "the devil number is not 667!")

    var d = "they are " ++ 4 ++ " apples in " ++ (2*2) ++ " baskets"
    @assert(d == "they are 4 apples in 4 baskets")
}

/**
A string and a rune can contain some *escape sequences* to specify special characters.
*/
#test
{
    // An escape sequence starts with `\`.
    {
        const a = "this is code ascii 0x00:   \0"
        const b = "this is code ascii 0x07:   \a"
        const c = "this is code ascii 0x08:   \b"
        const d = "this is code ascii 0x09:   \t"
        const e = "this is code ascii 0x0A:   \n"
        const f = "this is code ascii 0x0B:   \v"
        const g = "this is code ascii 0x0C:   \f"
        const h = "this is code ascii 0x0D:   \r"
        const i = "this is code ascii 0x22:   \""
        const j = "this is code ascii 0x27:   \'"
        const k = "this is code ascii 0x5C:   \\"
    }

    // An escape sequence can also be used to specify an ascii or a unicode value.
    {
        const a = "\x26"        // 1 byte, hexadecimal, extended ascii
        const b = "\u2626"      // 2 bytes, hexadecimal, unicode 16 bits
        const c = "\U26262626"  // 4 bytes, hexadecimal, unicode 32 bits

        const d = "\u2F46"
        #assert d == "⽆"
    }
}

/**
A *raw string* does not transform the escape sequences inside it.
*/
#test
{
    // A raw string starts and ends with the character `@`.
    {
        const a = @"\u2F46"@
        #assert a != "⽆"
        #assert a == @"\u2F46"@
    }

    // A raw string can be declared on several lines because `eol` is now part of the string.
    {
        const a = @"this is
                    a
                    string
                    "@

        // Every blanks before the ending mark `"@` will be removed from every other lines, so the string before is equivalent to :
        //
        // this is
        // a
        // string
    }
}

/**
A multiline string starts and ends with `"""`. Unlike raw strings, they still evaluate escape sequences.
*/
#test
{
    {
        const a = """this is
                     a
                     string
                     """

        // Equivalent to :
        // this is
        // a
        // string
    }

    // In a multiline or a raw string, if you end a line with `\`, the following <eol> will **not** be part of the string.
    {
        const a = """\
                     this is
                     a
                     string
                     """
        // Is equivalent to :
        // this is
        // a
        // string
    }
}

/**
A single *character* can be casted to every unsigned `s64` type.
*/
#test
{
    {
        var a = "0"'u8
        @assert(a == 48)
        @assert(@typeof(a)== @typeof(u8))

        var b = "1"'u16
        @assert(b == 49)
        @assert(@typeof(b)== @typeof(u16))

        var c = "2"'u32
        @assert(c == 50)
        @assert(@typeof(c)== @typeof(u32))

        var d = "3"'u64
        @assert(d == 51)
        @assert(@typeof(d)== @typeof(u64))

        var e = "4"'rune
        @assert(e == 52)
        @assert(@typeof(e)== @typeof(rune))
    }
}

/**
A string can be `null` if not defined.
*/
#test
{
    {
        var a: string
        @assert(a == null)
        a = "null"
        @assert(a != null)
        a = null
        @assert(a == null)
    }
}

/**
# @stringof
You can also use the instrinsic `@stringof` to return at compile time the result of a constant expression as a string.
*/
#test
{
    {
        const X = 1
        #assert @stringof(X) == "1"
        #assert @stringof(X + 10) == "11"
    }

    // You can also use `@nameof` to return the name of a variable, function etc.
    {
        const X = 1
        #assert @nameof(X) == "X"
    }
}