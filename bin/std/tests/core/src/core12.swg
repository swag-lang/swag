
using Core

//#[Swag.PrintBc]
#test
{
	let x = Env.getArgs()
	@assert(@countof(x) >= 1)
}