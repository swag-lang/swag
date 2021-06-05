//#[Swag.printbc]
#test
{
    var c: rune = "a"'rune
    c = c & ~0x20
    @assert(c == "A"'rune)

    c = c | 1
    c = c + 1
    c = c - 1
    c = c * 2
    c = c / 2
    c = c % 500
    c = c << 1
    c = c >> 1
    c = c ^ 1

    c |= 1
    c &= 1
    c += 1
    c -= 1
    c *= 1
    c /= 1
    c %= 1
    c <<= 1
    c >>= 1
    c ^= 1
}