#module module493
//#unittest pass semantic

//#[swag.printbc]
#test
{
	a := 10
	c := 20
	res := {&a, &c}
	res1 := res
	@assert(res1.0[0] == 10)
	@assert(res1.1[0] == 20)
}
