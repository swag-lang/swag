#test
{
    // Strings are UTF8
    {
        const a = "this is a chinese character: 是"
        @assert(a == "this is a chinese character: 是")

        const b = "this are some cyrillic characters: ӜИ"
        @assert(b == "this are some cyrillic characters: ӜИ")
    }

    // A character is unicode 32
    {
        const a = "是"'char
        @assert(a == "是"'char)
        @assert(@sizeof(a) == @sizeof(u32))
    }

    // You cannot index a string to get a character. You will retreive a byte instead, which will be a character only if the string is ascii
    {
        const a = "this is a chinese character: 是"
        const b = a[0]
        #assert(b == "t"'u8)
        #assert(@typeof(b) == @typeof(u8))

        // Here, the 'X' character in the middle does not have the index '1', because the chinese character before is
        // encoded in UTF8 with more than 1 byte
        const c = "是X是"
        #assert(c[1] != "X"'u8) // False because the byte number 1 is not the character 'X'
    }

    // Multiple strings are compiled as one
    {
        const a = "this is "   "a"   " string"
        #assert(a == "this is a string")
    }

    // You can concatenate some values if the values are known at compile time. Use the '~' character for that
    {
        let a = "the devil number is " ~666
        @assert(a == "the devil number is 666")

        const b = 666
        let c = "the devil number is not " ~(b + 1)~ "!"
        @assert(c == "the devil number is not 667!")

        let d = "they are " ~ 4 ~ " apples in " ~ (2*2) ~ " baskets"
        @assert(d == "they are 4 apples in 4 baskets")
    }
}