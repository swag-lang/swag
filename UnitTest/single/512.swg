#module module512
//#unittest pass semantic

//#[swag.printbytecode]
func T(xx: string, a: ...)
{
	i := 0'u32
	idx := 0
	while i < @prop(xx, count)
	{
		c := cast(char) xx[i]
		if c == '%'
		{
			i += 1
			switch cast(char) xx[i]
			{
			case 'd':
				@assert(cast(s32) a[idx] == 10)
				idx += 1
			case 's':
				@assert(cast(string) a[idx] == "toto")	
				idx += 1				
			}
		}
	
		i += 1
	}
}

//#[swag.printbytecode]
#test
{
	T("%s%d", "toto", 10)
}
