using Core.Utf8

struct ValidTest
{
    inT:      string
    outT:     rune
}

const validTests: [] ValidTest = [
    {"", RuneError},
    {"a", `a`},
    {"abc", `c`},
    {"Ж", `Ж`},
    {"ЖЖ", `Ж`},
    {"брэд-ЛГТМ", `М`},
    {"☺☻☹", `☹`},
    {"aa\xe2", RuneError},
    {"a\uFFFDb", `b`},
    {"\xF4\x8F\xBF\xF4", RuneError},
    {"\xF4\x8F\xBF\xed", RuneError},
    {"\xc0\x80", RuneError}
    /*{"\xF4\x90\x80\x80", RuneError},     // U+10FFFF+1; outT of range
	{"\xF7\xBF\xBF\xBF", RuneError},     // 0x1FFFFF; outT of range
	{"\xFB\xBF\xBF\xBF\xBF", RuneError}, // 0x3FFFFFF; outT of range
	{"\xc0\x80", false},             // U+0000 encoded inT two bytes: incorrect
	{"\xed\xa0\x80", false},         // U+D800 high surrogate (sic)
	{"\xed\xbf\xbf", false},         // U+DFFF low surrogate (sic)
    {"abcdabcdabcdabcd", true},
    {"abcdabcdabcdabcdX", true},
    {"abcdabcdabcdabcdX☺☻☹", true},
    {"abcdabcdabcdabcdX☺☻☹\xc0\x80", false},*/
    ]

#test
{
    foreach &it in validTests
    {
        @assert(decodeLastRune(it.inT).c == it.outT)
    }
}
