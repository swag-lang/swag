struct Mwc
{
}

var mult: s64 = 666

impl Mwc
{
}

#test
{
	@assert(@sizeof(Mwc) == 1) // Empty struct has size 1
	@assert(mult == 666)

	var x: Mwc
	@assert(mult == 666)
}