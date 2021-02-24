#global testerror

#import "core"
using core

#[swag.nocopy]
struct AA
{
}

//#[swag.printbc]
#test
{
    var arr: Array'AA
    var y: AA
    arr[0] = y
}