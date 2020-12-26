#import "core"
using core.console

//#[swag.printbc]
#test
{
    silent(true)
    ptr := &print
    ptr("toto")
    silent(false)
}