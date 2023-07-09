
using Core

//#[Swag.PrintBc]
#test
{
	var x = Env.getArgs()
	@assert(@countof(x) >= 1)
}