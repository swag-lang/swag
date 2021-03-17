


func F()->s32
{
	return 666;
}

//#[swag.printbc]
#test
{
	var a, b, c = F();
	@assert(a == 666);
	@assert(b == 666);
	@assert(c == 666);
}
