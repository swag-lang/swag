#module module489
//#unittest pass semantic

//#[swag.printbc]
#test
{
	res := {1, 2}
	@assert(res.0 == 1)
	@assert(res.1 == 2)
}
