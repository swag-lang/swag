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
{"\xc0\x80", RuneError}]

#test
{
    visit &it: validTests
    {
        @assert(decodeLastRune(it.inT).c == it.outT)
    }
}
