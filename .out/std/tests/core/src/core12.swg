
using core

//#[Swag.printbc]
#test
{
	x := env.getCommandLineArgs()
	@assert(@countof(x) >= 1)
}