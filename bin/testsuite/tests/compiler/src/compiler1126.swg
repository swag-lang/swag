func titi(line: u32 = #line)
{
	@assert(line == 8)
}

#test
{
	titi(#line)
}