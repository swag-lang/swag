
using core.utf8

struct ValidTest
{
	in:  string
	out: rune
}

const validTests: [] ValidTest = @[
	{"", CharError},
	{"a", "a"'rune},
	{"abc", "c"'rune},
	{"Ж", "Ж"'rune},
	{"ЖЖ", "Ж"'rune},
	{"брэд-ЛГТМ", "М"'rune},
	{"☺☻☹", "☹"'rune},
	{"aa\xe2", CharError},
	{"a\uFFFDb", "b"'rune},
	{"\xF4\x8F\xBF\xF4", CharError},
    {"\xF4\x8F\xBF\xed", CharError},
    {"\xc0\x80", CharError},
	/*{"\xF4\x90\x80\x80", CharError},     // U+10FFFF+1; out of range
	{"\xF7\xBF\xBF\xBF", CharError},     // 0x1FFFFF; out of range
	{"\xFB\xBF\xBF\xBF\xBF", CharError}, // 0x3FFFFFF; out of range
	{"\xc0\x80", false},             // U+0000 encoded in two bytes: incorrect
	{"\xed\xa0\x80", false},         // U+D800 high surrogate (sic)
	{"\xed\xbf\xbf", false},         // U+DFFF low surrogate (sic)
    {"abcdabcdabcdabcd", true},
    {"abcdabcdabcdabcdX", true},
    {"abcdabcdabcdabcdX☺☻☹", true},
    {"abcdabcdabcdabcdX☺☻☹\xc0\x80", false},*/
]

#test
{
    visit *it: validTests
    {
        @assert(decodeLastChar(it.in).c == it.out)
    }
}