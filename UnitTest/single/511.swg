#module module511
//#unittest pass semantic

//#[swag.printbc]
func T(xx: string, a: ...)
{
	@assert(cast(string) a[0] == "toto")
	@assert(cast(s32) a[1] == 10)
	
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
	T("%s%d", "toto", 10)
}
