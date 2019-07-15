#module module431
//#unittest pass semantic

#[swag.printbytecode]
#test
{
	len({1'u8,2,3,4})
	//len(cast!(const [..] u8)({0x01_02_03_04}))
}

//#[swag.printbytecode]
func len(buffer: const [..] u8)
{
	result := 0'u32
	total := 0;
	loop(buffer)
	{
		result += 1;
		total += cast!s32(buffer[index]);
	}
	
	@print(cast!s32(total))
	@print(" ")
	@print(cast!s32(result))
	@print("\n")
	//@assert(result == @prop(buffer, count));
}
