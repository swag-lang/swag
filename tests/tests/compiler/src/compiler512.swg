func(T) valueof(x: any)->T
{
	ptr := cast(*T) @dataof(x)
	return ptr[0]
}

//#[swag.printbc]
func toto(xx: string, a: ...)
{
	i := 0'u32
	idx := 0
	while i < @countof(xx)
	{
		c := cast(char) xx[i]
		if c == "%"'char
		{
			i += 1
			switch cast(char) xx[i]
			{
			case "d"'char:
				@assert(valueof's32(a[idx]) == 10)
				idx += 1
			case "s"'char:
				@assert(valueof'string(a[idx]) == "toto")	
				idx += 1				
			}
		}
	
		i += 1
	}
}

//#[swag.printbc]
#test
{
	toto("%s%d", "toto", 10)
}
