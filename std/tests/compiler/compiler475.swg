

//#[swag.printbc]
#test
{
	a := @[1, 2, 3, 0]
	b := F1(&a[0]);
	@assert(b == 3)
	b = F1(@dataof(a))
	@assert(b == 3)
}

/*func F(x: [..] s32, b: (s32)->s32)
{
}*/

func F1(b: *s32)->s32
{
	i := 0
	while(b[i] != 0)
		i += 1
	return i;
}