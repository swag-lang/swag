using Core

var spaceTest: [] rune = [
    0x09,
    0x0a,
    0x0b,
    0x0c,
    0x0d,
    0x20,
    0x85,
    0xA0,
    0x2000,
    0x3000]

var letterTest: [] rune = [
    0x41,
    0x61,
    0xaa,
    0xba,
    0xc8,
    0xdb,
    0xf9,
    0x2ec,
    0x535,
    0x620,
    0x6e6,
    0x93d,
    0xa15,
    0xb99,
    0xdc0,
    0xedd,
    0x1000,
    0x1200,
    0x1312,
    0x1401,
    0x2c00,
    0xa800,
    0xf900,
    0xfa30,
    0xffda,
    0xffdc,
    0x10000,
    0x10300,
    0x10400,
    0x20000,
    0x2f800,
    0x2fa1d]

var notletterTest: [] rune = [
    0x20,
    0x35,
    0x375,
    0x619,
    0x700,
    0x1885,
    0xfffe,
    0x1ffff,
    0x10ffff]

//#[Swag.PrintBc]
#test
{
    foreach c in spaceTest:
        @assert(Unicode.isSpace(c))
    foreach c in letterTest:
        @assert(Unicode.isLetter(c))
    foreach c in notletterTest:
        @assert(!Unicode.isLetter(c))
    @assert(Unicode.isLetter('是'))
    @assert(!Unicode.isLetter('‼'))

    @assert(Unicode.isNumber('௧'))
    @assert(Unicode.isNumber('൧'))
    @assert(Unicode.isNumber('𑄷'))
}
