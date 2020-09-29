public namespace std.ascii;
using swag

#[inline, constexpr]
{
    func isDigit(c: u8)         => ("0"'u8 <= c <= "9"'u8)
    func isBinDigit(c: u8)      => ("0"'u8 <= c <= "1"'u8)
    func isHexDigit(c: u8)      => ("0"'u8 <= c <= "9"'u8) || ("a"'u8 <= c <= "f"'u8) || ("A"'u8 <= c <= "F"'u8)
    func isUpper(c: u8)->bool   => ("A"'u8 <= c <= "Z"'u8)
    func isLower(c: u8)->bool   => ("a"'u8 <= c <= "z"'u8)
    func isLetter(c: u8)->bool  => ("a"'u8 <= c <= "z"'u8) || ("A"'u8 <= c <= "Z"'u8)
    func isLetterOrDigit(c: u8) => ("a"'u8 <= c <= "z"'u8) || ("A"'u8 <= c <= "Z"'u8) || ("0"'u8 <= c <= "9"'u8)
    func isWhiteSpace(c: u8)    => ("\x09"'u8 <= c <= "\x0d"'u8) || (c == " "'u8)
}

// Make an ascii buffer upper case
// A character that is not ascii will remain unchanged, so the function
// can be called on an utf8 buffer
func makeUpper(buffer: [..] u8)
{
    p := @dataof(buffer)
    loop @countof(buffer)
    {
        c := :p
        if (0x61 <= c <= 0x7A)
            :p -= 0x20
        p += 1
    }
}

// Make an ascii buffer lower case
// A character that is not ascii will remain unchanged, so the function
// can be called on an utf8 buffer
func makeLower(buffer: [..] u8)
{
    p := @dataof(buffer)
    loop @countof(buffer)
    {
        c := :p
        if (0x41 <= c <= 0x5A)
            :p += 0x20
        p += 1
    }
}
