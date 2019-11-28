#import "std"

#test
{
	x := std.crypto.crc32("toto")
	@assert(x == 0x10CCA4F1)
	y := std.crypto.crc32("toto la foule")
	@assert(y == 0xBDADD62A)
	z := std.crypto.crc32("toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto ")
	@assert(z == 0x445C76B7)
}