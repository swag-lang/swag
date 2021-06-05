func(T) valueof(x: any)->T
{
	ptr := cast(*T) @dataof(x)
	return ptr[0]
}

//#[Swag.printbc]
func toto(xx: string, a: ...)
{
	i := 0'u32
	idx := 0
	while i < @countof(xx)
	{
		c := cast(rune) xx[i]
		if c == "%"'rune
		{
			i += 1
			switch cast(rune) xx[i]
			{
			case "d"'rune:
				@assert(valueof's32(a[idx]) == 10)
				idx += 1
			case "s"'rune:
				@assert(valueof'string(a[idx]) == "toto")
				idx += 1
			}
		}

		i += 1
	}
}

//#[Swag.printbc]
#test
{
	toto("%s%d", "toto", 10)
}
