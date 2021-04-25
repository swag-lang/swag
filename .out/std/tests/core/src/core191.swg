
using core

struct CaseT
{
	cas:     s32
	in, out: rune
}

const UpperCase = 1
const LowerCase = 2
const TitleCase = 3

var caseTest: [] CaseT = @[
	// ASCII (special-cased so test carefully)
	{UpperCase, "\n"'rune, "\n"'rune},
	{UpperCase, "a"'rune, "A"'rune},
	{UpperCase, "A"'rune, "A"'rune},
	{UpperCase, "7"'rune, "7"'rune},
	{LowerCase, "\n"'rune, "\n"'rune},
	{LowerCase, "a"'rune, "a"'rune},
	{LowerCase, "A"'rune, "a"'rune},
	{LowerCase, "7"'rune, "7"'rune},
	{TitleCase, "\n"'rune, "\n"'rune},
	{TitleCase, "a"'rune, "A"'rune},
	{TitleCase, "A"'rune, "A"'rune},
	{TitleCase, "7"'rune, "7"'rune},

	// Latin-1: easy to read the tests!
	{UpperCase, 0x80, 0x80},
	{UpperCase, "Å"'rune, "Å"'rune},
	{UpperCase, "å"'rune, "Å"'rune},
	{LowerCase, 0x80, 0x80},
	{LowerCase, "Å"'rune, "å"'rune},
	{LowerCase, "å"'rune, "å"'rune},
	{TitleCase, 0x80, 0x80},
	{TitleCase, "Å"'rune, "Å"'rune},
	{TitleCase, "å"'rune, "Å"'rune},

	// 0131;LATIN SMALL LETTER DOTLESS I;Ll;0;L;;;;;N;;;0049;;0049
	{UpperCase, 0x0131, "I"'rune},
	{LowerCase, 0x0131, 0x0131},
	{TitleCase, 0x0131, "I"'rune},

	// 0133;LATIN SMALL LIGATURE IJ;Ll;0;L;<compat> 0069 006A;;;;N;LATIN SMALL LETTER I J;;0132;;0132
	{UpperCase, 0x0133, 0x0132},
	{LowerCase, 0x0133, 0x0133},
	{TitleCase, 0x0133, 0x0132},

	// 212A;KELVIN SIGN;Lu;0;L;004B;;;;N;DEGREES KELVIN;;;006B;
	{UpperCase, 0x212A, 0x212A},
	{LowerCase, 0x212A, "k"'rune},
	{TitleCase, 0x212A, 0x212A},

	// From an UpperLower sequence
	// A640;CYRILLIC CAPITAL LETTER ZEMLYA;Lu;0;L;;;;;N;;;;A641;
	{UpperCase, 0xA640, 0xA640},
	{LowerCase, 0xA640, 0xA641},
	{TitleCase, 0xA640, 0xA640},
	// A641;CYRILLIC SMALL LETTER ZEMLYA;Ll;0;L;;;;;N;;;A640;;A640
	{UpperCase, 0xA641, 0xA640},
	{LowerCase, 0xA641, 0xA641},
	{TitleCase, 0xA641, 0xA640},
	// A64E;CYRILLIC CAPITAL LETTER NEUTRAL YER;Lu;0;L;;;;;N;;;;A64F;
	{UpperCase, 0xA64E, 0xA64E},
	{LowerCase, 0xA64E, 0xA64F},
	{TitleCase, 0xA64E, 0xA64E},
	// A65F;CYRILLIC SMALL LETTER YN;Ll;0;L;;;;;N;;;A65E;;A65E
	{UpperCase, 0xA65F, 0xA65E},
	{LowerCase, 0xA65F, 0xA65F},
	{TitleCase, 0xA65F, 0xA65E},

	// From another UpperLower sequence
	// 0139;LATIN CAPITAL LETTER L WITH ACUTE;Lu;0;L;004C 0301;;;;N;LATIN CAPITAL LETTER L ACUTE;;;013A;
	{UpperCase, 0x0139, 0x0139},
	{LowerCase, 0x0139, 0x013A},
	{TitleCase, 0x0139, 0x0139},
	// 013F;LATIN CAPITAL LETTER L WITH MIDDLE DOT;Lu;0;L;<compat> 004C 00B7;;;;N;;;;0140;
	{UpperCase, 0x013f, 0x013f},
	{LowerCase, 0x013f, 0x0140},
	{TitleCase, 0x013f, 0x013f},
	// 0148;LATIN SMALL LETTER N WITH CARON;Ll;0;L;006E 030C;;;;N;LATIN SMALL LETTER N HACEK;;0147;;0147
	{UpperCase, 0x0148, 0x0147},
	{LowerCase, 0x0148, 0x0148},
	{TitleCase, 0x0148, 0x0147},

	// Lowercase lower than uppercase.
	// AB78;CHEROKEE SMALL LETTER GE;Ll;0;L;;;;;N;;;13A8;;13A8
	{UpperCase, 0xab78, 0x13a8},
	{LowerCase, 0xab78, 0xab78},
	{TitleCase, 0xab78, 0x13a8},
	{UpperCase, 0x13a8, 0x13a8},
	{LowerCase, 0x13a8, 0xab78},
	{TitleCase, 0x13a8, 0x13a8},

	// Last block in the 5.1.0 table
	// 10400;DESERET CAPITAL LETTER LONG I;Lu;0;L;;;;;N;;;;10428;
	{UpperCase, 0x10400, 0x10400},
	{LowerCase, 0x10400, 0x10428},
	{TitleCase, 0x10400, 0x10400},
	// 10427;DESERET CAPITAL LETTER EW;Lu;0;L;;;;;N;;;;1044F;
	{UpperCase, 0x10427, 0x10427},
	{LowerCase, 0x10427, 0x1044F},
	{TitleCase, 0x10427, 0x10427},
	// 10428;DESERET SMALL LETTER LONG I;Ll;0;L;;;;;N;;;10400;;10400
	{UpperCase, 0x10428, 0x10400},
	{LowerCase, 0x10428, 0x10428},
	{TitleCase, 0x10428, 0x10400},
	// 1044F;DESERET SMALL LETTER EW;Ll;0;L;;;;;N;;;10427;;10427
	{UpperCase, 0x1044F, 0x10427},
	{LowerCase, 0x1044F, 0x1044F},
	{TitleCase, 0x1044F, 0x10427},

	// First one not in the 5.1.0 table
	// 10450;SHAVIAN LETTER PEEP;Lo;0;L;;;;;N;;;;;
	{UpperCase, 0x10450, 0x10450},
	{LowerCase, 0x10450, 0x10450},
	{TitleCase, 0x10450, 0x10450},

	// Non-letters with case.
	{LowerCase, 0x2161, 0x2171},
	{UpperCase, 0x0345, 0x0399},
]

#test
{
    visit *c: caseTest
    {
        switch c.cas
        {
            case LowerCase:
                @assert(unicode.toLower(c.in) == c.out)
            case UpperCase:
                @assert(unicode.toUpper(c.in) == c.out)
            case TitleCase:
                @assert(unicode.toTitle(c.in) == c.out)
        }
    }
}