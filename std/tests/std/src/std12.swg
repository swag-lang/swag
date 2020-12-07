#import "std"
using std

//#[swag.printbc]
#test
{
	x := environment.getCommandLineArgs()
	@assert(@countof(x) >= 1)	
}