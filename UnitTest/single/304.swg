#module module304;
#version(FastTest)
{
	#unittest pass backend;
}
#else
{
	var g: bool = true;
	var g1: s32 = 666;
	var g2: u64 = 0xF000_000F;

	/*
	func compare: (str1: string, str2: string)-> bool
	{
		var count1 = @op(str1, count);
		var count2 = @op(str2, count);
		if(count1 != count2)
			return false;
			
		@print(cast(s32) count1);
		var src = @op(str1, data);
		var dst = @op(str2, data);
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
}