var spaceTest: []rune = [
	0x09,
	0x0a,
	0x0b,
	0x0c,
	0x0d,
	0x20,
	0x85,
	0xA0,
	0x2000,
	0x3000,
]

//#[Swag.PrintBc]
#test
{
    var g = 0
    foreach c in spaceTest: g += 1
    @assert(g == 10)
}
