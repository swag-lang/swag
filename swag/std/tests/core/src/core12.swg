#import "core"
using core

//#[swag.printbc]
#test
{
	x := env.getCommandLineArgs()
	@assert(@countof(x) >= 1)
}