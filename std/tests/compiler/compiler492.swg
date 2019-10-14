
//#unittest pass semantic

//#[swag.printbc]
#test
{
	a := 10
	c := 20
	res := {&a, &c}
	@assert(res.val0[0] == 10)
	@assert(res.val1[0] == 20)
}
