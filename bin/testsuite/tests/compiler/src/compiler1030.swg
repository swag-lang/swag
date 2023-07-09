#global testerror
//#[Swag.PrintBc]
#test
{
	var x = 10
	@init(&x)(0.0)
	@assert(x == 0)
}