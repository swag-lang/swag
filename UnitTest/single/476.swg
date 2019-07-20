#module module476
#unittest pass semantic

//#[swag.printbytecode]
#test
{
	a := {1, 2, 3, 0}
	F(a, &T)
}

func T(val: s32)->s32
{
	return val + 1;
}

func F(x: [..] s32, b: (s32)->s32) ->s32
{
	total := 0
	loop(x) total += b(x[index])	
	return total
}
