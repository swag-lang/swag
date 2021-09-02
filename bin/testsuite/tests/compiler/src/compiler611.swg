


func(op: s32) count()->s32
{
	return op
}

//#[Swag.PrintBc]
#test
{
	x := count'(1)()
	y := count'(2)()
	@assert(x == 1)
	@assert(y == 2)
}