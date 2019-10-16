


typealias ret: {s32, s32}

func toto()->ret
{
	return {1, 2}
}

#test
{
	a:= toto()
	@assert(a.val0 == 1)
	@assert(a.val1 == 2)
}