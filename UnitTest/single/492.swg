#module module492
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	a := 10
	c := 20
	res := {&a, &c}
	@assert(res.0[0] == 10)
	@assert(res.1[0] == 20)
}
