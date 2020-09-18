#module "std56"
#unittest error

#import "std"
using std

//#[swag.printbc]
#test
{
    var x: BitArray = [true, false, true, false]
    visit *ptr: x
    {
        @print("XX\n")
    }
}