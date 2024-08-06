#global testerror "Err0599"
//#[Swag.PrintBc]
#test
{
	var x = 10
	@init(&x, 1)(0.0)
	@assert(x == 0)
}