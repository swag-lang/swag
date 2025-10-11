func(T) valueof(x: any)->T
{
    let ptr = cast(*T) @dataof(x)
    return dref ptr
}

//#[Swag.PrintBc]
func toto(xx: string, a: ...)
{
    @assert(valueof'string(a[0]) == "toto")
    @assert(valueof's32(a[1]) == 10)

    /*
	var i = 0'u32
	var idx = 0
	while(i < @countof(xx))
	{
		var c = xx[i]
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

//#[Swag.PrintBc]
#test
{
    toto("%s%d", "toto", 10)
}
