#import "std"
using std

var spaceTest: []char = @[
	0x09,
	0x0a,
	0x0b,
	0x0c,
	0x0d,
	0x20,
	0x85,
	0xA0,
	0x2000,
	0x3000,
]

//#[swag.printbc]
#test
{
    visit c: spaceTest
    {
        @assert(unicode32.isSpace(c))
    }
}
