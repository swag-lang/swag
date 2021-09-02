//#[Swag.PrintBc]
#test
{
	cst :=  0x01_02_03_04
	var slice : [..] u8 = @mkslice(cast(*u8)&cst, 4)
	len(slice)
}

//#[Swag.PrintBc]
func len(buffer: const [..] u8)
{
	result := 0
	total := 0
	loop buffer
	{
		result += 1
		total += cast(s32) buffer[@index]
	}

	@assert(total == 10)
	@assert(result == 4)
}
