
using Core

//#[Swag.PrintBc]
#test
{
	x := Env.getArgs()
	@assert(@countof(x) >= 1)
}