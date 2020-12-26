


func(T) valueof(x: any)->T
{
	ptr := cast(*T) @dataof(x)
	return ptr[0]
}

//#[swag.printbc]
func toto(xx: string, a: ...)
{
	@assert(valueof'string(a[0]) == "toto")
	@assert(valueof's32(a[1]) == 10)
	
	/*
	i := 0'u32
	idx := 0
	while(i < @countof(xx))
	{
		c := xx[i]
		if(c == cast(u8) '%')
		{
			i += 1
			switch(xx[i])
			{
			case cast(u8) 'd':
				@print(cast(s32) a[idx])
				idx += 1
			case cast(u8) 's':
				@print(cast(string) a[idx])	
				idx += 1				
			}
		}
	
		i += 1
	}*/
}

//#[swag.printbc]
#test
{
	toto("%s%d", "toto", 10)
}
