using Core.Utf8

struct RuneCountTest
{
    inT:      string
    outT:     u64
}

const runecounttests: [] RuneCountTest = [
    {"abcd", 4},
    {"☺☻☹", 3},
    {"1,2,3,4", 7},
    {"\xe2\x00", 2},
    {"\xe2\x80", 2},
    {"a\xe2\x80", 3}]

#test
{
    foreach it in runecounttests
    {
        @assert(countRunes(it.inT) == it.outT)
    }
}
