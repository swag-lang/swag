#import "core"
using core

//#[swag.printbc]
#test
{
	x := environment.getCommandLineArgs()
	@assert(@countof(x) >= 1)
}