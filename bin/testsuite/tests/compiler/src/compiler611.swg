


func(op: s32) count()->s32
{
	return op
}

//#[Swag.PrintBc]
#test
{
	var x = count'(1)()
	var y = count'(2)()
	@assert(x == 1)
	@assert(y == 2)
}