typealias ret = {a,b: s32}

func toto()->ret
{
	return {1, 2}
}

#test
{
	var a = toto()
	@assert(a.item0 == 1)
	@assert(a.item1 == 2)
}