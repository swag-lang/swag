#import "std"
using std.hash

#test
{
	x := Crc32.hash("toto")
	@assert(x == 0x10CCA4F1)
	y := Crc32.hash("toto la foule")
	@assert(y == 0xBDADD62A)
	z := Crc32.hash("toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto ")
	@assert(z == 0x445C76B7)
}