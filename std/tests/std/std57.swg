#import "std"
using std

//#[swag.printbc]
#test
{
    var x: BitArray = [true, false, true, false]
    visit val: x
    {
        @print(cast(s64) val)
        @print(" ")
    }
}