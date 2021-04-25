
using core.utf8

struct RuneCountTest
{
	in:  string
	out: uint
}

const runecounttests: [] RuneCountTest = @[
	{"abcd", 4},
	{"☺☻☹", 3},
	{"1,2,3,4", 7},
	{"\xe2\x00", 2},
	{"\xe2\x80", 2},
	{"a\xe2\x80", 3},
]

#test
{
    visit it: runecounttests
    {
        @assert(countRunes(it.in) == it.out)
    }
}