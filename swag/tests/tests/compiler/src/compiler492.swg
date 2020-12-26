//#[swag.printbc]
#test
{
	a := 10
	c := 20
	res := @{&a, &c}
	@assert(res.item0[0] == 10)
	@assert(res.item1[0] == 20)
}
