#import "std"
using std

#test
{
	x := crc32.slice("toto")
	@assert(x == 0x10CCA4F1)
	y := crc32.slice("toto la foule")
	@assert(y == 0xBDADD62A)
	z := crc32.slice("toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto ")
	@assert(z == 0x445C76B7)
}