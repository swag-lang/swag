#import "std"
using std

#[swag.printbc]
#test
{
    var x: BitArray = [true, false, true, false]
    visit x
    {
        //@print("XX\n")
    }
}