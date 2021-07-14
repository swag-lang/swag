;

/*
func compare (str1: string, str2: string)-> bool
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

//#[Swag.PrintBc]
func compare (str: string)
{
	var count = @countof(str);
	@assert(count == 4);
}

//#[Swag.PrintBc]
#test
{
	compare("toto");
}
