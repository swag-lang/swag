using Core.Hash

#test
{
	var x = Crc32.compute("toto")
	@assert(x == 0x10CCA4F1)
	var y = Crc32.compute("toto la foule")
	@assert(y == 0xBDADD62A)
	var z = Crc32.compute("toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto toto ")
	@assert(z == 0x445C76B7)
}