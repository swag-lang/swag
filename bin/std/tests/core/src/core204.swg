using Core.Utf8

struct ValidTest
{
    inT:      string
    outT:     bool
}

const validTests: [] ValidTest = [
    {"", true},
    {"a", true},
    {"abc", true},
    {"Ж", true},
    {"ЖЖ", true},
    {"брэд-ЛГТМ", true},
    {"☺☻☹", true},
    {"aa\xe2", false},
    //{string([]byte{66, 250}), false},
    //{string([]byte{66, 250, 67}), false},
    {"a\uFFFDb", true},
    {"\xF4\x8F\xBF\xBF", true},
    // U+10FFFF
    {"\xF4\x90\x80\x80", false},
    // U+10FFFF+1; outT of range
    {"\xF7\xBF\xBF\xBF", false},
    // 0x1FFFFF; outT of range
    {"\xFB\xBF\xBF\xBF\xBF", false},
    // 0x3FFFFFF; outT of range
    {"\xc0\x80", false},
    // U+0000 encoded inT two bytes: incorrect
    {"\xed\xa0\x80", false},
    // U+D800 high surrogate (sic)
    {"\xed\xbf\xbf", false},
    // U+DFFF low surrogate (sic)
    {"abcdabcdabcdabcd", true},
    {"abcdabcdabcdabcdX", true},
    {"abcdabcdabcdabcdX☺☻☹", true},
    {"abcdabcdabcdabcdX☺☻☹\xc0\x80", false}]

#test
{
    foreach &it in validTests
    {
        @assert(isValid(it.inT) == it.outT)
    }
}
