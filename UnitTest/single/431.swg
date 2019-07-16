#module module431
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	len(cast(const[..]u8) {0x01_02_03_04} )
}

//#[swag.printbytecode]
func len(buffer: const [..] u8)
{
	result := 0
	total := 0
	loop buffer
	{
		result += 1
		total += cast(s32) buffer[index]
	}
	
	@assert(total == 10)
	@assert(result == 4)
}
