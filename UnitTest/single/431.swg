#module module431
//#unittest pass semantic

#[swag.printbytecode]
#test
{
	len(cast!(const [..] u8)({1,2,3}))
}

func len(buffer: const [..] u8)
{
	/*result := 0'u32
	loop(buffer)
		result += 1;
	@print(cast!s32(result))
	@assert(result == @prop(buffer, count));*/
}
