struct ValidTest
{
	in:  string
	out: bool
}

const validTests: [] ValidTest = @[
	{"", true},
	{"a", true},
	{"abc", true},
	{"Ж", true},
	{"ЖЖ", true},
	{"брэд-ЛГТМ", true},
	{"☺☻☹", true},
	{"aa\xe2", false},
	{"a\uFFFDb", true},
	{"\xF4\x8F\xBF\xBF", true},      // U+10FFFF
	{"\xF4\x90\x80\x80", false},     // U+10FFFF+1; out of range
	{"\xF7\xBF\xBF\xBF", false},     // 0x1FFFFF; out of range
	{"\xFB\xBF\xBF\xBF\xBF", false}, // 0x3FFFFFF; out of range
	{"\xc0\x80", false},             // U+0000 encoded in two bytes: incorrect
	{"\xed\xa0\x80", false},         // U+D800 high surrogate (sic)
	{"\xed\xbf\xbf", false},         // U+DFFF low surrogate (sic)
]

//#[Swag.PrintBc]
#test
{
    @assert(validTests[1].in == "a")
}