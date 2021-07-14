
using Core

//#[Swag.PrintBc]
#test
{
	x := Env.getCommandLineArgs()
	@assert(@countof(x) >= 1)
}