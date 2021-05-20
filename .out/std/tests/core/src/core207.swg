// https://github.com/golang/go/blob/master/src/hash/crc32/crc32_test.go

using core.Hash

struct ValidTest
{
	out1: u32
    out2: u32
	in1: string
    in2: string
    in3: string
}

const validTests: [] ValidTest = @[
	{0x0, 0x0, "", "crc\x01ʇ\x91M\x00\x00\x00\x00", "crc\x01wB\x84\x81\x00\x00\x00\x00"},
	{0xe8b7be43, 0xc1d04330, "a", "crc\x01ʇ\x91M\x00\x00\x00\x00", "crc\x01wB\x84\x81\x00\x00\x00\x00"},
	{0x9e83486d, 0xe2a22936, "ab", "crc\x01ʇ\x91M跾C", "crc\x01wB\x84\x81\xc1\xd0C0"},
	{0x352441c2, 0x364b3fb7, "abc", "crc\x01ʇ\x91M跾C", "crc\x01wB\x84\x81\xc1\xd0C0"},
	{0xed82cd11, 0x92c80a31, "abcd", "crc\x01ʇ\x91M\x9e\x83Hm", "crc\x01wB\x84\x81\xe2\xa2)6"},
	{0x8587d865, 0xc450d697, "abcde", "crc\x01ʇ\x91M\x9e\x83Hm", "crc\x01wB\x84\x81\xe2\xa2)6"},
	{0x4b8e39ef, 0x53bceff1, "abcdef", "crc\x01ʇ\x91M5$A\xc2", "crc\x01wB\x84\x816K?\xb7"},
	{0x312a6aa6, 0xe627f441, "abcdefg", "crc\x01ʇ\x91M5$A\xc2", "crc\x01wB\x84\x816K?\xb7"},
	{0xaeef2a50, 0xa9421b7, "abcdefgh", "crc\x01ʇ\x91M\xed\x82\xcd\x11", "crc\x01wB\x84\x81\x92\xc8\n1"},
	{0x8da988af, 0x2ddc99fc, "abcdefghi", "crc\x01ʇ\x91M\xed\x82\xcd\x11", "crc\x01wB\x84\x81\x92\xc8\n1"},
	{0x3981703a, 0xe6599437, "abcdefghij", "crc\x01ʇ\x91M\x85\x87\xd8e", "crc\x01wB\x84\x81\xc4P֗"},
	{0x6b9cdfe7, 0xb2cc01fe, "Discard medicine more than two years old.", "crc\x01ʇ\x91M\xfd\xe5\xc2J", "crc\x01wB\x84\x81S\"(\xe0"},
	{0xc90ef73f, 0xe28207f, "He who has a shady past knows that nice guys finish last.", "crc\x01ʇ\x91M\x01ǋ+", "crc\x01wB\x84\x81'\xdaR\x15"},
	{0xb902341f, 0xbe93f964, "I wouldn't marry him with a ten foot pole.", "crc\x01ʇ\x91M\x9d\x13\xce\x10", "crc\x01wB\x84\x81\xc3\xed\xabG"},
	{0x42080e8, 0x9e3be0c3, "Free! Free!/A trip/to Mars/for 900/empty jars/Burma Shave", "crc\x01ʇ\x91M-\xed\xf7\x94", "crc\x01wB\x84\x81\xce\xceb\x81"},
	{0x154c6d11, 0xf505ef04, "The days of the digital watch are numbered.  -Tom Stoppard", "crc\x01ʇ\x91MOa\xa5\r", "crc\x01wB\x84\x81\xd3s\x9dP"},
	{0x4c418325, 0x85d3dc82, "Nepal premier won't resign.", "crc\x01ʇ\x91M\xa8S9\x85", "crc\x01wB\x84\x81{\x90\x8a\x14"},
	{0x33955150, 0xc5142380, "For every action there is an equal and opposite government program.", "crc\x01ʇ\x91Ma\xe9>\x86", "crc\x01wB\x84\x81\xaa@\xc4\x1c"},
	{0x26216a4b, 0x75eb77dd, "His money is twice tainted: 'taint yours and 'taint mine.", "crc\x01ʇ\x91M\\\x1an\x88", "crc\x01wB\x84\x81W\a8Z"},
	{0x1abbe45e, 0x91ebe9f7, "There is no reason for any individual to have a computer in their home. -Ken Olsen, 1977", "crc\x01ʇ\x91M\xb7\xf5\xf2\xca", "crc\x01wB\x84\x81\xc4o\x9d\x85"},
	{0xc89a94f7, 0xf0b1168e, "It's a tiny change to the code and not completely disgusting. - Bob Manchek", "crc\x01ʇ\x91M\x84g1\xe8", "crc\x01wB\x84\x81#\x98\f\xab"},
	{0xab3abe14, 0x572b74e2, "size:  a.out:  bad magic", "crc\x01ʇ\x91M\x8a\x0f\xad\b", "crc\x01wB\x84\x81\x80\xc9n\xd8"},
	{0xbab102b6, 0x8a58a6d5, "The major problem is with sendmail.  -Mark Horton", "crc\x01ʇ\x91M\a\xf0\xb3\x15", "crc\x01wB\x84\x81liS\xcc"},
	{0x999149d7, 0x9c426c50, "Give me a rock, paper and scissors and I will move the world.  CCFestoon", "crc\x01ʇ\x91M\x0fa\xbc.", "crc\x01wB\x84\x81\xdb͏C"},
	{0x6d52a33c, 0x735400a4, "If the enemy is within range, then so are you.", "crc\x01ʇ\x91My\x1b\x99\xf8", "crc\x01wB\x84\x81\xaaB\x037"},
	{0x90631e8d, 0xbec49c95, "It's well we cannot hear the screams/That we create in others' dreams.", "crc\x01ʇ\x91M\bqfY", "crc\x01wB\x84\x81\x16y\xa1\xd2"},
	{0x78309130, 0xa95a2079, "You remind me of a TV show, but that's all right: I watch it anyway.", "crc\x01ʇ\x91M\xbdO,\xc2", "crc\x01wB\x84\x81f&\xc5\xe4"},
	{0x7d0a377f, 0xde2e65c5, "C is as portable as Stonehedge!!", "crc\x01ʇ\x91M\xf7\xd6\x00\xd5", "crc\x01wB\x84\x81de\\\xf8"},
	{0x8c79fd79, 0x297a88ed, "Even if I could be Shakespeare, I think I should still choose to be Faraday. - A. Huxley", "crc\x01ʇ\x91Ml+\xb8\xa7", "crc\x01wB\x84\x81\xbf\xd6S\xdd"},
	{0xa20b7167, 0x66ed1d8b, "The fugacity of a constituent in a mixture of gases at a given temperature is proportional to its mole fraction.  Lewis-Randall Rule", "crc\x01ʇ\x91M<lR[", "crc\x01wB\x84\x81{\xaco\xb1"},
	{0x8e0bb443, 0xdcded527, "How can you write a big system without C++?  -Paul Glick", "crc\x01ʇ\x91M\x0e\x88\x89\xed", "crc\x01wB\x84\x813\xd7C\u007f"},
]

#test
{
    visit *it: validTests
    {
        res := Crc32.compute(it.in1)
        @assert(res == it.out1)
    }
}