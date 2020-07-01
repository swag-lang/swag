#import "std"
using std.console

//#[swag.printbc]
#test
{
    silent(true)   
    ptr := &write
    ptr("toto")
    silent(false)
}