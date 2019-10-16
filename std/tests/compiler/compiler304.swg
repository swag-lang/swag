var g: bool = true;
var g1: s32 = 666;
var g2: u64 = 0xF000_000F;

/*
func compare: (str1: string, str2: string)-> bool
{
	var count1 = @countof(str1);
	var count2 = @countof(str2);
	if(count1 != count2)
		return false;
			
	@print(cast(s32) count1);
	var src = @dataof(str1);
	var dst = @dataof(str2);
	while(src[0] != 0)
	{
		if(src[0] != dst[0])
			return false;
		src += 1;
		dst += 1;
	}
		
	return true;
}*/

//#[swag.printbc]
#test
{
	@assert(g == true);
	@assert(g1 == 666);
	@assert(g2 == 0xF000_000F);
}
