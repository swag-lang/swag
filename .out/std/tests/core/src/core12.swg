
using core

//#[Swag.printbc]
#test
{
	x := Env.getCommandLineArgs()
	@assert(@countof(x) >= 1)
}