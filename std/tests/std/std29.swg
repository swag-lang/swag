#import "std"
using std

#test
{
	x := hash.crc32("toto")
	@assert(x == 0x10CCA4F1)
	y := hash.crc32("toto la foule")
	@assert(y == 0xBDADD62A)
	z := hash.crc32("toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto ")
	@assert(z == 0x445C76B7)
}