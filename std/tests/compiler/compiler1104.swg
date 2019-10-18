struct Mwc
{
}

impl Mwc
{
	var mult: s64 = 666
}

#test
{
	@assert(@sizeof(Mwc) == 0)
	@assert(Mwc.mult == 666)
	
	var x: Mwc
	@assert(x.mult == 666)
}