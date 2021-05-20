


//#[Swag.printbc]
#test
{
	a := 10
	c := 20
	res := @{&a, &c}
	res1 := res
	@assert(res1.item0[0] == 10)
	@assert(res1.item1[0] == 20)
}
