#module module570
//#unittest pass semantic

typealias ret: {s32, s32}

func toto()->ret
{
	return {1, 2}
}

#test
{
	a:= toto()
	@assert(a.0 == 1)
	@assert(a.1 == 2)
}